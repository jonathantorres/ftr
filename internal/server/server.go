package server

import (
	"errors"
	"fmt"
	"log"
	"net"

	"github.com/jonathantorres/ftr/internal/conf"
)

const (
	ControlPort                    = 21
	DefaultName                    = "localhost"
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
	s.Host = s.Conf.ServerName
	s.Port = s.Conf.Port
	l, err := s.getServerListener()
	if err != nil {
		return err
	}
	for {
		conn, err := l.AcceptTCP()
		if err != nil {
			log.Printf("accept error: %s\n", err)
			continue
		}
		go s.handleClient(conn)
	}
	return nil
}

func (s *Server) getServerListener() (*net.TCPListener, error) {
	// check if a host or an address is provided for the server
	var ipAddrs []net.IP
	ip := net.ParseIP(s.Host)
	if ip == nil {
		// a hostname was provided
		addrs, err := net.LookupHost(s.Host)
		if err != nil {
			return nil, err
		}
		if len(addrs) == 0 {
			return nil, errors.New("No addresses could be found for the server")
		}
		for _, addr := range addrs {
			ipAddrs = append(ipAddrs, net.ParseIP(addr))
		}
	} else {
		// user specified an IP address, and that sole address here
		ipAddrs = append(ipAddrs, ip)
	}
	if len(ipAddrs) == 0 {
		return nil, errors.New("The server address could not be parsed")
	}

	// let's try every address until a binding succeeds
	var l *net.TCPListener
	var err error
	for _, ip := range ipAddrs {
		laddr := &net.TCPAddr{
			IP:   ip,
			Port: s.Port,
		}
		l, err = net.ListenTCP("tcp", laddr)
		if err != nil {
			// binding failed, let's try the next one
			continue
		}
		// binding is successfull, we are done
		break
	}
	if l == nil {
		return nil, errors.New("Could not bind an address for the server")
	}
	return l, nil
}

func (s *Server) handleClient(conn *net.TCPConn) {
	err := sendResponse(conn, StatusCodeServiceReady, "") // welcome message
	if err != nil {
		log.Printf("error response: %s\n", err)
		return
	}
	session := &Session{
		controlConn: conn,
		server:      s,
	}
	session.start()
}

func (s *Server) findOpenAddr(useIPv6 bool) (*net.TCPAddr, error) {
	proto := "tcp"
	if useIPv6 {
		proto += "6"
	}
	addr, err := net.ResolveTCPAddr(proto, fmt.Sprintf("%s:0", s.Host))
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

func sendResponse(conn *net.TCPConn, statusCode uint16, extraMsg string) error {
	codeMsg := GetStatusCodeMessage(statusCode)
	respMsg := fmt.Sprintf("%d %s %s\n", statusCode, codeMsg, extraMsg)
	log.Printf(respMsg)
	_, err := conn.Write([]byte(respMsg))
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
