package main

import (
	"flag"
	"fmt"
	"log"
	"os"

	"github.com/jonathantorres/ftr/internal/conf"
	"github.com/jonathantorres/ftr/internal/server"
)

// TODO: look into adding a "prefix" value that will specify
// the prefix of the server (a location in the filesystem ex. /usr/local/ftr)
// this location will contain the configuration files and all the log files
// will be stored here, this value should be set at build time,
// the default value will be /usr/local/ftr, this value can be overriden
// by using the command line flag -p (prefix)

// TODO: fix these command line arguments
// make them more sensical and usable
var hostFlag = flag.String("host", server.DefaultHost, "The host of the server")
var portFlag = flag.Int("port", server.ControlPort, "The port number for the control connection")
var confFlag = flag.String("conf", server.DefaultConf, "The location of the configuration file")

func main() {
	flag.Usage = usage
	flag.Parse()

	// logging
	log.SetPrefix("ftr: ")

	// TODO: install signals to manage the server
	//       SIGINT, SIGTERM and SIGQUIT to shutdown the server
	//       SIGHUB to reload the configuration file

	config, err := conf.Load(*confFlag)
	if err != nil {
		log.Fatalf("server conf error: %s\n", err)
	}
	s := &server.Server{
		Host: *hostFlag,
		Port: *portFlag,
		Conf: config,
	}
	err = s.Start()
	if err != nil {
		log.Fatalf("server error: %s\n", err)
		os.Exit(1)
	}
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage: ftr -[hv] [-p port] [-h host] [-c conf]\n")
	flag.PrintDefaults()
}
