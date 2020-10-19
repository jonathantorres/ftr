package server

import (
	"net"
	"fmt"
	"io"
	"os"
	"strings"
)

const (
	ControlPort = 9999 // this should be 21
	DefaultHost = "localhost"
)

const (
	defaultCmdSize = 512
)

type Server struct {
	Host string
	Port int
}

func (s *Server) Start() error {
	l, err := net.Listen("tcp", fmt.Sprintf("%s:%d", s.Host, s.Port))
	if err != nil {
		return err
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Fprintf(os.Stderr, "accept error:  %s\n", err)
			continue
		}
		go handleClient(conn)
	}
	return nil
}

func handleClient(conn net.Conn) {
	err := sendResponse(conn, 220) // welcome message
	if err != nil {
		fmt.Fprintf(os.Stderr, "error response: %s\n", err)
		return
	}
	for {
		clientCmd := make([]byte, defaultCmdSize)
		_, err := conn.Read(clientCmd)
		if err != nil {
			if err == io.EOF {
				fmt.Fprintf(os.Stderr, "connection finished by client %s\n", err)
			} else {
				fmt.Fprintf(os.Stderr, "error read: %s\n", err)
				sendResponse(conn, 500)
			}
			conn.Close()
			break
		}
		err = handleCommand(clientCmd, conn)
		if err != nil {
			sendResponse(conn, 500)
			continue
		}
	}
}

func sendResponse(conn net.Conn, statusCode uint16) error {
	codeMsg, err := GetStatusCodeMessage(statusCode)
	var code uint16
	var msg string
	if err != nil {
		code = 500
		msg = err.Error()
	} else {
		code = statusCode
		msg = codeMsg
	}
	msg = fmt.Sprintf("%d %s\n", code, msg)
	_, err = conn.Write([]byte(msg))
	if err != nil {
		return err
	}
	return nil
}

func handleCommand(clientCmd []byte, conn net.Conn) error {
	clientCmdStr := trimCommandLine(clientCmd)
	cmdParts := strings.Split(clientCmdStr, " ")
	cmd := strings.TrimSpace(cmdParts[0])

	if len(cmdParts) == 1 {
		return execCommand(cmd, conn, "")
	} else if len(cmdParts) > 1 {
		return execCommand(cmd, conn, cmdParts[1:]...)
	}
	return sendResponse(conn, 500)
}

func runCommandUser(conn net.Conn, username string) error {
	// TODO: logic to check for the username
	return sendResponse(conn, 331)
}

func runCommandPassword(conn net.Conn, pass string) error {
	// TODO: logic to check for the password
	return sendResponse(conn, 230)
}

func runUninmplemented(conn net.Conn) error {
	return sendResponse(conn, 502)
}

func execCommand(cmd string, conn net.Conn, cmdArgs...string) error {
	var err error = nil

	switch cmd {
	case CommandUser:
		err = runCommandUser(conn, cmdArgs[0])
	case CommandPassword:
		err = runCommandPassword(conn, cmdArgs[0])
	default:
		err = runUninmplemented(conn)
	}
	return err
}

func trimCommandLine(clientCmd []byte) string {
	trimmedCommand := ""
	for _, b := range clientCmd {
		if rune(b) != 0x00 && rune(b) != '\r' && rune(b) != '\n' {
			trimmedCommand += string(b)
		}
	}
	return trimmedCommand
}
