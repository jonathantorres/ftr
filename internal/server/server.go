package server

import (
	"fmt"
	"net"
	"os"
)

const (
	ControlPort                    = 9999 // this should be 21
	DefaultHost                    = "localhost"
	defaultCmdSize                 = 512
	TransferTypeAscii TransferType = "A"
	TransferTypeImage TransferType = "I"
)

type TransferType string

type Server struct {
	Host string
	Port int
	Conf *ServerConf
}

type ServerConf struct {
	Root  string
	Users []*User
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
		go s.handleClient(conn)
	}
	return nil
}

func (s *Server) handleClient(conn net.Conn) {
	err := sendResponse(conn, 220, "") // welcome message
	if err != nil {
		fmt.Fprintf(os.Stderr, "error response: %s\n", err)
		return
	}
	session := &Session{
		controlConn: conn,
		server:      s,
	}
	session.start()
}

func sendResponse(conn net.Conn, statusCode uint16, respMsg string) error {
	codeMsg, err := GetStatusCodeMessage(statusCode)
	var code uint16
	if err != nil {
		code = 500
		respMsg = err.Error()
	} else {
		code = statusCode
		if respMsg == "" {
			respMsg = codeMsg
		}
	}
	respMsg = fmt.Sprintf("%d %s\n", code, respMsg)
	_, err = conn.Write([]byte(respMsg))
	if err != nil {
		return err
	}
	return nil
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
