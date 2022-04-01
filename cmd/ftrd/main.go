package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/signal"
	"strings"
	"syscall"

	"github.com/jonathantorres/ftr/internal/conf"
	"github.com/jonathantorres/ftr/internal/server"
)

// TODO: look into adding a "prefix" value that will specify
// the prefix of the server (a location in the filesystem ex. /usr/local/ftr)
// this location will contain the configuration files and all the log files
// will be stored here, this value should be set at build time,
// the default value will be /usr/local/ftr, this value can be overriden
// by using the command line flag -p (prefix)
var prefix = "/home/jonathan/dev/ftr/"

func main() {
	// make sure the prefix ends in "/"
	if !strings.HasSuffix(prefix, "/") {
		prefix += "/"
	}
	serverF, confF, portF := parseFlags()

	log.SetPrefix("ftr: ")

	go handleSignals()

	config, err := conf.Load(confF)
	if err != nil {
		log.Fatalf("server conf error: %s\n", err)
	}
	s := &server.Server{
		Host: serverF,
		Port: portF,
		Conf: config,
	}
	err = s.Start()
	if err != nil {
		log.Fatalf("server error: %s\n", err)
	}
}

func parseFlags() (string, string, int) {
	var (
		serverF string
		portF   int
		confF   string
	)

	const (
		serverD = "The default name of the server"
		portD   = "The port number for the control connection"
		confD   = "The location of the configuration file"
	)
	flag.StringVar(&serverF, "server", server.DefaultName, serverD)
	flag.StringVar(&serverF, "s", server.DefaultName, serverD)
	flag.IntVar(&portF, "port", server.ControlPort, portD)
	flag.IntVar(&portF, "p", server.ControlPort, portD)
	flag.StringVar(&confF, "conf", prefix+server.DefaultConf, confD)
	flag.StringVar(&confF, "c", prefix+server.DefaultConf, confD)
	flag.Usage = usage
	flag.Parse()

	var (
		serverUsed bool
		portUsed   bool
	)

	flag.Visit(func(f *flag.Flag) {
		if f.Name == "server" || f.Name == "s" {
			serverUsed = true
		}
		if f.Name == "port" || f.Name == "p" {
			portUsed = true
		}
	})
	// use zero value if the flag was not specified in the command line
	if !serverUsed {
		serverF = ""
	}
	if !portUsed {
		portF = 0
	}
	return serverF, confF, portF
}

func handleSignals() {
	sigs := make(chan os.Signal, 1)
	signal.Notify(sigs, syscall.SIGINT, syscall.SIGTERM, syscall.SIGQUIT, syscall.SIGHUP)

	s := <-sigs
	switch s {
	case syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT:
		// TODO: shutdown the server gracefully
		log.Print("shutting down server...")
		os.Exit(0)
	case syscall.SIGHUP:
		// TODO: this probably shouldn't terminate the program
		// it should shutdown the server, and restart it
		// so that it reloads the configuration file
		log.Print("reloading configuration file...")
		os.Exit(0)
	}
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage: ftr -[hv] [-s server] [-p port] [-c conf]\n")
	flag.PrintDefaults()
}
