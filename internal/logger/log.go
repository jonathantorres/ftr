package logger

import (
	"errors"
	"log"
	"os"

	"github.com/jonathantorres/ftr/internal/conf"
)

const Prefix = "ftr:"

type Log struct {
	l         *log.Logger
	stderrLog *log.Logger
}

func (l *Log) Printf(format string, v ...interface{}) {
	l.l.Printf(format, v...)

	if l.stderrLog != nil {
		l.stderrLog.Printf(format, v...)
	}
}

func (l *Log) Print(v ...interface{}) {
	l.l.Print(v...)

	if l.stderrLog != nil {
		l.stderrLog.Print(v...)
	}
}

func (l *Log) Println(v ...interface{}) {
	l.l.Println(v...)

	if l.stderrLog != nil {
		l.stderrLog.Println(v...)
	}
}

func Load(config *conf.Conf) (*Log, *Log, error) {
	if config == nil {
		return nil, nil, errors.New("configuration was not provided")
	}

	if config.ErrorLog == "" {
		return nil, nil, errors.New("path to error log file is required")
	}

	if config.AccessLog == "" {
		return nil, nil, errors.New("path to access log file is required")
	}

	fe, err := os.OpenFile(config.ErrorLog, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return nil, nil, err
	}

	fa, err := os.OpenFile(config.AccessLog, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	if err != nil {
		return nil, nil, err
	}

	stderrLog := log.New(os.Stderr, Prefix, log.LstdFlags)
	errLog := &Log{
		l:         log.New(fe, Prefix, log.LstdFlags),
		stderrLog: stderrLog,
	}

	accessLog := &Log{
		l:         log.New(fa, Prefix, log.LstdFlags),
		stderrLog: stderrLog,
	}

	return errLog, accessLog, nil
}
