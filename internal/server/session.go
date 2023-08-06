package server

import (
	"fmt"
	"io"
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
	id                 int
	user               *User
	server             *Server
	tType              TransferType
	passMode           bool
	controlConn        *net.TCPConn
	dataConn           net.Conn
	dataConnPort       uint16
	dataConnChan       chan struct{}
	cwd                string
	renameFrom         string
	transferInProgress bool
}

func (s *Session) start() {
	for {
		clientCmd := make([]byte, defaultCmdSize)
		_, err := s.controlConn.Read(clientCmd)
		if err != nil {
			if err == io.EOF {
				s.server.LogAcc("connection finished by client %s", err)
			} else {
				s.server.LogErr("read error: %s", err)
				s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
			}

			s.controlConn.Close()
			break
		}

		err = s.handleCommand(clientCmd)
		if err != nil {
			s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
			continue
		}
	}
}

// end the current session, this will assume
// that the data connection is already closed
// and that there are no transfers in progress
func (s *Session) end() {
	if s.controlConn != nil {
		err := s.controlConn.CloseWrite()
		if err != nil {
			s.server.LogErr("error when closing the control connection: %s", err)
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
			s.server.LogErr("data conn: accept error: %s", err)
			return
		}

		go s.handleDataTransfer(conn, l)
	}()

	return nil
}

func (s *Session) connectToDataConn(port uint16, useIPv6 bool) error {
	proto := "tcp"
	if useIPv6 {
		proto += "6"
	}

	c, err := net.Dial(proto, fmt.Sprintf("%s:%d", s.server.Host, port))
	if err != nil {
		return err
	}

	s.dataConn = c
	s.dataConnPort = port
	s.dataConnChan = make(chan struct{})

	go func() {
		var sig struct{}
		// send signal to command that the connection is ready
		s.dataConnChan <- sig

		// wait until the command finishes, then close the connection
		<-s.dataConnChan

		s.dataConn = nil
		s.dataConnPort = 0
		s.dataConnChan = nil

		defer c.Close()
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

	return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
}

func (s *Session) execCommand(cmd string, cmdArgs string) error {
	var err error = nil
	s.server.LogAcc("%s %s\n", cmd, cmdArgs)

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
	case CommandFileNames:
		err = runCommandFileNames(s, cmdArgs)
	case CommandRetrieve:
		err = runCommandRetrieve(s, cmdArgs)
	case CommandAcceptAndStore, CommandStoreFile:
		err = runCommandAcceptAndStore(s, cmdArgs, false)
	case CommandAppend:
		err = runCommandAcceptAndStore(s, cmdArgs, true)
	case CommandSystemType:
		err = runCommandSystemType(s)
	case CommandChangeParent, CommandChangeToParentDir:
		err = runCommandChangeParent(s)
	case CommandMakeDir, CommandMakeADir:
		err = runCommandMakeDir(s, cmdArgs)
	case CommandRemoveDir:
		err = runCommandRemoveDir(s, cmdArgs)
	case CommandDelete:
		err = runCommandDelete(s, cmdArgs)
	case CommandExtPassMode:
		err = runCommandExtPassMode(s, cmdArgs)
	case CommandPort:
		err = runCommandPort(s, cmdArgs)
	case CommandExtAddrPort:
		err = runCommandExtPort(s, cmdArgs)
	case CommandHelp:
		err = runCommandHelp(s, cmdArgs)
	case CommandNoOp:
		err = runCommandNoOp(s)
	case CommandAllo:
		err = runCommandAllo(s)
	case CommandAccount:
		err = runCommandAccount(s, cmdArgs)
	case CommandSite:
		err = runCommandSite(s)
	case CommandMode:
		err = runCommandMode(s, cmdArgs)
	case CommandAbort:
		err = runCommandAbort(s)
	case CommandFileStruct:
		err = runCommandFileStructure(s, cmdArgs)
	case CommandServerStatus:
		err = runCommandServerStatus(s, cmdArgs)
	case CommandRenameFrom:
		err = runCommandRenameFrom(s, cmdArgs)
	case CommandRenameTo:
		err = runCommandRenameTo(s, cmdArgs)
	case CommandReinit:
		err = runCommandReinit(s)
	case CommandQuit:
		err = runCommandQuit(s)
	default:
		err = runUninmplemented(s)
	}

	return err
}

func (s *Session) loggedIn() bool {
	if s.user == nil {
		return false
	}

	return true
}
