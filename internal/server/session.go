package server

import (
	"fmt"
	"io"
	"net"
	"os"
	"strings"
)

// the current active session
type Session struct {
	user         *User
	server       *Server
	tType        TransferType
	passMode     bool
	controlConn  net.Conn
	dataConn     net.Conn
	dataConnPort uint16
	dataConnChan chan struct{}
	cwd          string
}

// the current user logged in for this session
type User struct {
	Username string
	Password string
	Root     string
}

func (s *Session) start() {
	for {
		clientCmd := make([]byte, defaultCmdSize)
		_, err := s.controlConn.Read(clientCmd)
		if err != nil {
			if err == io.EOF {
				fmt.Fprintf(os.Stderr, "connection finished by client %s\n", err)
			} else {
				fmt.Fprintf(os.Stderr, "error read: %s\n", err)
				sendResponse(s.controlConn, 500, "")
			}
			s.controlConn.Close()
			break
		}
		err = s.handleCommand(clientCmd)
		if err != nil {
			sendResponse(s.controlConn, 500, "")
			continue
		}
	}
}

func (s *Session) openDataConn(port uint16) error {
	l, err := net.Listen("tcp", fmt.Sprintf("%s:%d", s.server.Host, port))
	if err != nil {
		return err
	}
	s.dataConnPort = port
	s.dataConnChan = make(chan struct{})
	go func() {
		conn, err := l.Accept()
		if err != nil {
			fmt.Fprintf(os.Stderr, "data conn: accept error:  %s\n", err)
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
	cmdParts := strings.Split(clientCmdStr, " ")
	cmd := strings.TrimSpace(cmdParts[0])

	if len(cmdParts) == 1 {
		return s.execCommand(cmd, "")
	} else if len(cmdParts) > 1 {
		return s.execCommand(cmd, cmdParts[1:]...)
	}
	return sendResponse(s.controlConn, 500, "")
}

func (s *Session) execCommand(cmd string, cmdArgs ...string) error {
	var err error = nil
	fmt.Fprintf(os.Stdout, "cmd: %s\n", cmd)
	switch cmd {
	case CommandUser:
		err = runCommandUser(s, cmdArgs[0])
	case CommandPassword:
		err = runCommandPassword(s, cmdArgs[0])
	case CommandPrintDir:
		err = runCommandPrintDir(s)
	case CommandChangeDir:
		err = runCommandChangeDir(s, cmdArgs[0])
	case CommandType:
		err = runCommandType(s, cmdArgs[0])
	case CommandPassive:
		err = runCommandPasv(s)
	case CommandList:
		err = runCommandList(s, cmdArgs[0])
	case CommandRetrieve:
		err = runCommandRetrieve(s, cmdArgs[0])
	case CommandAcceptAndStore:
		err = runCommandAcceptAndStore(s, cmdArgs[0])
	case CommandSystemType:
		err = runCommandSystemType(s)
	case CommandChangeParent:
	case CommandChangeToParentDir:
		err = runCommandChangeParent(s)
	default:
		err = runUninmplemented(s)
	}
	return err
}
