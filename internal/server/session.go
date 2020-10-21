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
	user        *User
	server      *Server
	tType       TransferType
	passMode    bool
	controlConn net.Conn
	dataConn    net.Conn
	cwd         string
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
	switch cmd {
	case CommandUser:
		err = runCommandUser(s, cmdArgs[0])
	case CommandPassword:
		err = runCommandPassword(s, cmdArgs[0])
	case CommandPrintDir:
		err = runCommandPrintDir(s)
	case CommandChangeDir:
		err = runCommandChangeDir(s, cmdArgs[0])
	// case CommandType:
	// 	err = runCommandType(s, cmdArgs[0])
	// case CommandPassive:
	// 	err = runCommandPasv(s)
	// case CommandPort:
	// 	err = runCommandPort(s, cmdArgs[0])
	// case CommandList:
	// 	err = runCommandList(s)
	default:
		err = runUninmplemented(s)
	}
	return err
}
