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

const version = "0.1.0"

func main() {
	confF := parseFlags()

	// make sure the prefix ends in "/"
	if !strings.HasSuffix(prefix, "/") {
		prefix += "/"
	}
	log.SetPrefix("ftr: ")

	config, err := conf.Load(confF)
	if err != nil {
		log.Fatalf("server conf error: %s", err)
	}
	s := &server.Server{
		Conf: config,
	}
	go handleSignals(s)

	err = s.Start()
	if err != nil {
		log.Fatalf("server error: %s", err)
	}
}

func parseFlags() string {
	var (
		versionF bool
		helpF    bool
		testF    bool
		prefixF  string
		confF    string
	)
	const (
		versionD = "Print the server version and exit"
		helpD    = "Print the help contents and exit"
		testD    = "Test the configuration file and exit"
		prefixD  = "Set the prefix path"
		confD    = "Set the configuration file"
	)
	flag.BoolVar(&versionF, "version", false, versionD)
	flag.BoolVar(&versionF, "v", false, versionD)
	flag.BoolVar(&helpF, "help", false, helpD)
	flag.BoolVar(&helpF, "h", false, helpD)
	flag.BoolVar(&testF, "test", false, testD)
	flag.BoolVar(&testF, "t", false, testD)
	flag.StringVar(&prefixF, "prefix", prefix, prefixD)
	flag.StringVar(&prefixF, "p", prefix, prefixD)
	flag.StringVar(&confF, "conf", prefix+server.DefaultConf, confD)
	flag.StringVar(&confF, "c", prefix+server.DefaultConf, confD)
	flag.Usage = usage
	flag.Parse()

	// just print the version and exit
	if versionF {
		fmt.Fprintf(os.Stderr, "ftr version v%s\n", version)
		os.Exit(0)
	}

	// just print the help and exit
	if helpF {
		usage()
		os.Exit(0)
	}

	// test the configuration file and exit
	if testF {
		// TODO: test the configuration file
		fmt.Fprintf(os.Stderr, "testing configuration file...Done\n")
		os.Exit(0)
	}

	var prefixUsed bool

	flag.Visit(func(f *flag.Flag) {
		if f.Name == "prefix" || f.Name == "p" {
			prefixUsed = true
		}
	})
	// if prefix was set on the command line,
	// then the location of the configuration
	// will be based on this prefix
	if prefixUsed {
		prefix = prefixF
		confF = prefix + server.DefaultConf
	}

	return confF
}

func handleSignals(serv *server.Server) {
	sig := make(chan os.Signal, 1)
	signal.Notify(sig, syscall.SIGINT, syscall.SIGTERM, syscall.SIGQUIT, syscall.SIGHUP)

	s := <-sig
	switch s {
	case syscall.SIGINT,
		syscall.SIGTERM,
		syscall.SIGQUIT:
		serv.Shutdown()
	case syscall.SIGHUP:
		// TODO: this probably shouldn't terminate the program
		// it should shutdown the server, and restart it
		// so that it reloads the configuration file
		log.Print("reloading configuration file...")
		os.Exit(0)
	}
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage: ftr -[htv] [-p prefix] [-c conf]\n")
	flag.PrintDefaults()
}
