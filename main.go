package main

import (
	"flag"
	"fmt"
	"github.com/jonathantorres/lima/internal/server"
	"io"
	"net"
	"os"
)

// Control connection:
// The server should listen on port 21 (control port)
// In this port a client with attempt to login into the server

// Data connection:
// Only support passive mode (for now)
// Once a client is connected and logged into the server (using the control connection)
// The client will execute commands to it
// If one of these commands is to download a file or to upload a file
// The server will create a data connection and a port, for the client to connect to
// This connection will handle the transfer (download or upload) of the file

const controlPort = 9999 // this should be 21

var portFlag = flag.Int("port", controlPort, "The port number for the control connection")

func main() {
	flag.Parse()
	l, err := net.Listen("tcp", fmt.Sprintf("localhost:%d", *portFlag))
	if err != nil {
		fmt.Fprintf(os.Stderr, "error listening %s\n", err)
		os.Exit(1)
	}
	for {
		conn, err := l.Accept()
		if err != nil {
			fmt.Fprintf(os.Stderr, "error accepting connection %s\n", err)
			continue
		}
		go handleClient(conn)
	}
}

func sendResponse(conn net.Conn, statusCode uint16) error {
	codeMsg, err := server.GetStatusCodeMessage(statusCode)
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

func handleClient(conn net.Conn) {
	err := sendResponse(conn, 220)
	if err != nil {
		fmt.Fprintf(os.Stderr, "error response: %s", err)
		return
	}
	for {
		clientData := make([]byte, 512)
		_, err := conn.Read(clientData)
		if err != nil {
			if err == io.EOF {
				fmt.Fprintf(os.Stderr, "connection finished by client %s\n", err)
			} else {
				fmt.Fprintf(os.Stderr, "error reading data from connection %s\n", err)
				sendResponse(conn, 500)
			}
			conn.Close()
			break
		}
		// TODO: parse the command in clientData
		// Do the operation and respond accordingly
		_, err = conn.Write([]byte(fmt.Sprintf("You requested: %s. Done!\n", clientData)))
		if err != nil {
			fmt.Fprintf(os.Stderr, "error writing to connection %s\n", err)
			sendResponse(conn, 500)
			continue
		}
		sendResponse(conn, 331)
	}
}
