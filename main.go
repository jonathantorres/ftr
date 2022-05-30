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
	"github.com/jonathantorres/ftr/internal/logger"
	"github.com/jonathantorres/ftr/internal/server"
)

const version = "0.1.0"

func main() {
	// make sure the prefix ends in "/"
	if !strings.HasSuffix(server.Prefix, "/") {
		server.Prefix += "/"
	}
	confF := parseFlags()
	log.SetPrefix(logger.Prefix)

	for {
		config, err := conf.Load(confF, server.Prefix)
		if err != nil {
			log.Fatalf("server configuration error: %s", err)
		}
		logE, logA, err := logger.Load(config)
		if err != nil {
			log.Fatalf("server logging error: %s", err)
		}
		s := &server.Server{
			Conf: config,
			LogA: logA,
			LogE: logE,
		}
		go handleSignals(s)

		err = s.Start()
		if err != nil {
			log.Fatalf("server error: %s", err)
		}
		if s.IsReloading {
			continue
		}
		break
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
	flag.StringVar(&prefixF, "prefix", server.Prefix, prefixD)
	flag.StringVar(&prefixF, "p", server.Prefix, prefixD)
	flag.StringVar(&confF, "conf", server.Prefix+server.DefaultConf, confD)
	flag.StringVar(&confF, "c", server.Prefix+server.DefaultConf, confD)
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
		server.Prefix = prefixF
		confF = server.Prefix + server.DefaultConf
	}

	// test the configuration file and exit
	if testF {
		fmt.Fprintf(os.Stderr, "testing configuration file...")
		_, err := conf.Load(confF, server.Prefix)
		if err != nil {
			fmt.Fprintf(os.Stderr, "failed: %s\n", err)
			os.Exit(1)
		}
		fmt.Fprintf(os.Stderr, "OK.\n")
		os.Exit(0)
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
		err := serv.Reload()
		if err != nil {
			// there was a problem reloading the configuration file
			// the server will keep running with the old configuration
			// fire up this goroutine again so we can keep handling signals
			go handleSignals(serv)
		}
	}
}

func usage() {
	fmt.Fprintf(os.Stderr, "Usage: ftr -[htv] [-p prefix] [-c conf]\n")
	flag.PrintDefaults()
}
