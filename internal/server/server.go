package server

import (
	"errors"
	"fmt"
	"math/rand"
	"net"
	"sync"
	"time"

	"github.com/jonathantorres/ftr/internal/conf"
	"github.com/jonathantorres/ftr/internal/logger"
)

var Prefix = ""

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
	Host           string
	Port           int
	Conf           *conf.Conf
	IsReloading    bool
	LogA           *logger.Log
	LogE           *logger.Log
	sessions       map[int]*Session
	listener       *net.TCPListener
	shutdownC      chan struct{}
	isShuttingDown bool
}

func (s *Server) Start() error {
	s.LogA.Print("server starting...")
	s.Host = s.Conf.ServerName
	s.Port = s.Conf.Port
	l, err := s.getServerListener()
	if err != nil {
		return err
	}
	s.listener = l
	s.shutdownC = make(chan struct{}, 1)
	s.LogA.Print("server started OK, waiting for connections")
	for {
		conn, err := l.AcceptTCP()
		if err != nil {
			select {
			case <-s.shutdownC:
				// nothing to do,
				// the server is shutting down
			default:
				s.LogE.Printf("accept error: %s", err)
			}
			break
		}
		go s.handleClient(conn)
	}
	// wait for the shutdown to finish
	<-s.shutdownC
	return nil
}

func (s *Server) Shutdown() error {
	s.LogA.Print("shutting down server...")
	s.isShuttingDown = true
	var wg sync.WaitGroup
	for _, session := range s.sessions {
		wg.Add(1)
		go func(s *Session) {
			defer wg.Done()
			runCommandQuit(s)
		}(session)
	}
	wg.Wait()
	if s.listener != nil {
		s.shutdownC <- struct{}{}
		err := s.listener.Close()
		if err != nil {
			s.LogE.Printf("error closing the main listener: %s", err)
		}
	}
	s.LogA.Print("server shutdown complete")
	s.shutdownC <- struct{}{}
	return nil
}

func (s *Server) Reload() error {
	s.LogA.Print("reloading configuration file...")
	_, err := conf.Load(Prefix+DefaultConf, Prefix)
	if err != nil {
		s.LogE.Printf("configuration file error: %s", err)
		return err
	}
	s.LogA.Print("configuration file OK.")
	s.IsReloading = true
	s.Shutdown()
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
	if s.isShuttingDown {
		err := conn.Close()
		if err != nil {
			s.LogE.Printf("error closing accepted connection on shutdown: %s", err)
		}
		return
	}
	err := s.sendResponse(conn, StatusCodeServiceReady, "") // welcome message
	if err != nil {
		s.LogE.Printf("error response: %s", err)
		return
	}
	rand.Seed(time.Now().UnixNano())
	session := &Session{
		controlConn: conn,
		server:      s,
		id:          rand.Int(),
	}
	if s.sessions == nil {
		s.sessions = make(map[int]*Session)
	}
	s.sessions[session.id] = session
	session.start()
	delete(s.sessions, session.id)
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

func (s *Server) sendResponse(conn *net.TCPConn, statusCode uint16, extraMsg string) error {
	codeMsg := GetStatusCodeMessage(statusCode)
	respMsg := fmt.Sprintf("%d %s %s\n", statusCode, codeMsg, extraMsg)
	s.LogA.Printf(respMsg)
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
