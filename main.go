package main

import (
	"encoding/json"
	"flag"
	"fmt"
	"io/ioutil"
	"os"

	"github.com/jonathantorres/ftr/internal/server"
)

var hostFlag = flag.String("host", server.DefaultHost, "The host of the server")
var portFlag = flag.Int("port", server.ControlPort, "The port number for the control connection")
var confFlag = flag.String("conf", server.DefaultConf, "The location of the configuration file")

func main() {
	flag.Parse()
	conf, err := loadConf(*confFlag)
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

func loadConf(configLoc string) (*server.ServerConf, error) {
	file, err := ioutil.ReadFile(configLoc)
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
