package main

import (
	"flag"
	"fmt"
	"log"
	"os"
	"os/exec"
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
		daemonF  bool
		prefixF  string
		confF    string
	)

	flag.BoolVar(&versionF, "v", false, "Show the server version and exit")
	flag.BoolVar(&helpF, "h", false, "Show the help contents and exit")
	flag.BoolVar(&testF, "t", false, "Test the configuration file and exit")
	flag.BoolVar(&daemonF, "d", false, "Run the server in the background (as a daemon)")
	flag.StringVar(&prefixF, "p", server.Prefix, "Set the path of the prefix")
	flag.StringVar(&confF, "c", server.Prefix+server.DefaultConf, "Use the specified configuration file")
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
		if f.Name == "p" {
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

	// run as a daemon
	if daemonF {
		cmd := exec.Command(os.Args[0])
		err := cmd.Start()

		if err != nil {
			fmt.Fprintf(os.Stderr, "there was a problem starting the daemon: %s\n")
			os.Exit(1)
		}

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
	fmt.Fprintf(os.Stderr, "Usage: ftr -[dhtv] [-p prefix] [-c conf]\n\n")
	fmt.Fprintf(os.Stderr, "Options:\n")
	fmt.Fprintf(os.Stderr, "  -h\t\t: This help menu\n")
	fmt.Fprintf(os.Stderr, "  -v\t\t: Show server version and exit\n")
	fmt.Fprintf(os.Stderr, "  -t\t\t: Test the configuration file and exit\n")
	fmt.Fprintf(os.Stderr, "  -d\t\t: Run the server in the background (as a daemon)\n")
	fmt.Fprintf(os.Stderr, "  -p prefix\t: Set the path of the prefix\n")
	fmt.Fprintf(os.Stderr, "  -c filename\t: Use the specified configuration file\n")
}
