package conf

import "errors"

const (
	equalSign      = '='
	openBracket    = '{'
	closingBracket = '}'
	commentSign    = '#'

	serverOpt    = "server"
	rootOpt      = "root"
	portOpt      = "port"
	userOpt      = "user"
	usernameOpt  = "username"
	passwordOpt  = "password"
	errorLogOpt  = "error_log"
	accessLogOpt = "access_log"
	includeOpt   = "include"
)

type Conf struct {
	ServerName string
	Port       int
	Root       string
	ErrorLog   string
	AccessLog  string
	Users      []*User
}

type User struct {
	Username string
	Password string
	Root     string
}

func Load(confPath string) (*Conf, error) {
	return &Conf{}, errors.New("not implemented")
}
