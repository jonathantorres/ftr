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
		// TODO: parse the command in clientCmd
		// Do the operation and respond accordingly
		_, err = conn.Write([]byte(fmt.Sprintf("You requested: %s. Done!\n", clientCmd)))
		if err != nil {
			fmt.Fprintf(os.Stderr, "error writing to connection %s\n", err)
			sendResponse(conn, 500)
			continue
		}
		sendResponse(conn, 331)
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
	cmdStr := string(clientCmd)
	cmdParts := strings.Split(cmdStr, " ")

	if len(cmdParts) == 1 {
		execCommand(cmdParts[0], conn, "")
	} else if len(cmdParts) > 1 {
		execCommand(cmdParts[0], conn, cmdParts[1:]...)
	} else {
		sendResponse(conn, 500)
	}
	return nil
}

func execCommand(cmd string, conn net.Conn, cmdArgs...string) error {
	fmt.Fprintf(os.Stderr, "trying to exec %s, ", cmd)
	if len(cmdArgs) > 0 {
		fmt.Fprintf(os.Stderr, "with args: %s\n", cmdArgs)
	} else {
		fmt.Fprintf(os.Stderr, "\n")
	}
	return nil
}
