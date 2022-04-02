package server

import (
	"fmt"
	"io"
	"log"
	"net"
)

// the current user logged in for this session
type User struct {
	Username string
	Password string
	Root     string
}

// the current active session
type Session struct {
	user         *User
	server       *Server
	tType        TransferType
	passMode     bool
	controlConn  *net.TCPConn
	dataConn     net.Conn
	dataConnPort uint16
	dataConnChan chan struct{}
	cwd          string
}

func (s *Session) start() {
	for {
		clientCmd := make([]byte, defaultCmdSize)
		_, err := s.controlConn.Read(clientCmd)
		if err != nil {
			if err == io.EOF {
				log.Printf("connection finished by client %s\n", err)
			} else {
				log.Printf("read error: %s\n", err)
				sendResponse(s.controlConn, StatusCodeUnknownErr, "")
			}
			s.controlConn.Close()
			break
		}
		err = s.handleCommand(clientCmd)
		if err != nil {
			sendResponse(s.controlConn, StatusCodeUnknownErr, "")
			continue
		}
	}
}

func (s *Session) openDataConn(port uint16, useIPv6 bool) error {
	proto := "tcp"
	if useIPv6 {
		proto += "6"
	}
	l, err := net.Listen(proto, fmt.Sprintf("%s:%d", s.server.Host, port))
	if err != nil {
		return err
	}
	s.dataConnPort = port
	s.dataConnChan = make(chan struct{})
	go func() {
		conn, err := l.Accept()
		if err != nil {
			log.Printf("data conn: accept error: %s\n", err)
			return
		}
		go s.handleDataTransfer(conn, l)
	}()
	return nil
}

func (s *Session) handleDataTransfer(conn net.Conn, l net.Listener) {
	s.dataConn = conn

	var sig struct{}
	// send signal to command that the connection is ready
	s.dataConnChan <- sig

	// wait until the command finishes, then close the connection
	<-s.dataConnChan

	s.dataConn = nil
	s.dataConnPort = 0
	s.dataConnChan = nil
	defer conn.Close()
	defer l.Close()
}

func (s *Session) handleCommand(clientCmd []byte) error {
	clientCmdStr := trimCommandLine(clientCmd)
	cmd := ""
	cmdParams := ""
	foundFirstSpace := false
	for _, r := range clientCmdStr {
		if !foundFirstSpace && r == ' ' {
			foundFirstSpace = true
			continue
		}
		if foundFirstSpace {
			cmdParams += string(r)
		} else {
			cmd += string(r)
		}
	}
	if cmdParams == "" {
		return s.execCommand(cmd, "")
	} else {
		return s.execCommand(cmd, cmdParams)
	}
	return sendResponse(s.controlConn, StatusCodeUnknownErr, "")
}

func (s *Session) execCommand(cmd string, cmdArgs string) error {
	var err error = nil
	log.Printf("cmd: %s\n", cmd)
	switch cmd {
	case CommandUser:
		err = runCommandUser(s, cmdArgs)
	case CommandPassword:
		err = runCommandPassword(s, cmdArgs)
	case CommandPrintDir:
		err = runCommandPrintDir(s)
	case CommandChangeDir:
		err = runCommandChangeDir(s, cmdArgs)
	case CommandType:
		err = runCommandType(s, cmdArgs)
	case CommandPassive:
		err = runCommandPasv(s)
	case CommandList:
		err = runCommandList(s, cmdArgs)
	case CommandRetrieve:
		err = runCommandRetrieve(s, cmdArgs)
	case CommandAcceptAndStore:
		err = runCommandAcceptAndStore(s, cmdArgs)
	case CommandSystemType:
		err = runCommandSystemType(s)
	case CommandChangeParent, CommandChangeToParentDir:
		err = runCommandChangeParent(s)
	case CommandMakeDir, CommandMakeADir:
		err = runCommandMakeDir(s, cmdArgs)
	case CommandDelete:
		err = runCommandDelete(s, cmdArgs)
	case CommandExtPassMode:
		err = runCommandExtPassMode(s, cmdArgs)
	case CommandExtAddrPort:
		// TODO: implement at some point
		err = runUninmplemented(s)
	default:
		err = runUninmplemented(s)
	}
	return err
}
