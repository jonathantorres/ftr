package server

import (
	"errors"
	"fmt"
	"net"
	"os"

	"github.com/jonathantorres/ftr/internal/conf"
)

const (
	ControlPort                    = 21
	DefaultHost                    = "localhost"
	DefaultConf                    = "ftr.conf"
	defaultCmdSize                 = 512
	TransferTypeAscii TransferType = "A"
	TransferTypeImage TransferType = "I"
)

type TransferType string

type Server struct {
	Host string
	Port int
	Conf *conf.Conf
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

func findOpenAddr() (*net.TCPAddr, error) {
	addr, err := net.ResolveTCPAddr("tcp", "localhost:0")
	if err != nil {
		return nil, err
	}
	l, err := net.ListenTCP("tcp", addr)
	if err != nil {
		return nil, err
	}
	addr, ok := l.Addr().(*net.TCPAddr)
	if !ok {
		return nil, errors.New("tcp address could not be resolved")
	}
	defer l.Close()
	return addr, nil
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
