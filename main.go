package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"github.com/jonathantorres/gftp/internal/server"
	"io/ioutil"
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

var hostFlag = flag.String("host", server.DefaultHost, "The host of the server")
var portFlag = flag.Int("port", server.ControlPort, "The port number for the control connection")

func main() {
	flag.Parse()
	conf, err := loadConf()
	if err != nil {
		fmt.Fprintf(os.Stderr, "server conf error: %s\n", err)
		os.Exit(1)
	}
	s := &server.Server{
		Host: *hostFlag,
		Port: *portFlag,
		Conf: conf,
	}
	err = s.Start()
	if err != nil {
		fmt.Fprintf(os.Stderr, "server error: %s\n", err)
		os.Exit(1)
	}
}

func loadConf() (*server.ServerConf, error) {
	file, err := ioutil.ReadFile("./gftp.json") // Add this as a cmd line option?
	if err != nil {
		return nil, err
	}
	conf := &server.ServerConf{}
	err = json.Unmarshal(file, conf)
	if err != nil {
		return nil, err
	}
	return conf, nil
}
