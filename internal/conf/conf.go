package conf

import (
	"bufio"
	"bytes"
	"log"
	"os"
	"strconv"
)

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
	Users      []User // TODO: should we user a pointer here? an array of User pointers?
}

// TODO: should we use a pointer for the "user" param?
func (c *Conf) addUser(user User) {
	if c.Users == nil {
		c.Users = make([]User, 0, 10)
	}
	c.Users = append(c.Users, user)
}

func (c *Conf) addOption(opName, opValue string) {
	switch opName {
	case serverOpt:
		c.ServerName = opValue
	case rootOpt:
		c.Root = opValue
	case errorLogOpt:
		c.ErrorLog = opValue
	case accessLogOpt:
		c.AccessLog = opValue
	case portOpt:
		// TODO: I don't think we should ignore this error
		p, _ := strconv.Atoi(opValue)
		c.Port = p
	}
}

type User struct {
	Username string
	Password string
	Root     string
}

func (u *User) addOption(opName, opValue string) {
	switch opName {
	case usernameOpt:
		u.Username = opValue
	case passwordOpt:
		u.Password = opValue
	case rootOpt:
		u.Root = opValue
	}
}

func Load(confPath string) (*Conf, error) {
	file, err := openAndStripComments(confPath)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	file, err = parseIncludes(file)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	err = checkForSyntaxErrors(file)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	conf, err := buildServerConf(file)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	return conf, nil
}

func buildServerConf(file []byte) (*Conf, error) {
	r := bytes.NewReader(file)
	scanner := bufio.NewScanner(r)
	var insideUserCmd bool
	conf := &Conf{}
	var curUser *User

	for scanner.Scan() {
		line := scanner.Bytes()
		if bytes.ContainsRune(line, equalSign) {
			// this is a line with an option
			ops := bytes.Split(line, []byte{byte(equalSign)})
			opName := string(bytes.TrimSpace(ops[0]))
			opValue := string(bytes.TrimSpace(ops[1]))
			if insideUserCmd {
				// option for the current user
				curUser.addOption(opName, opValue)
			} else {
				// top level or global option
				conf.addOption(opName, opValue)
			}
		} else if bytes.Contains(line, []byte(userOpt)) {
			// this is a line with a user command
			insideUserCmd = true
			curUser = &User{}
		} else if bytes.ContainsRune(line, closingBracket) {
			// closing bracket for a user command
			if insideUserCmd {
				conf.addUser(*curUser)
				curUser = nil
				insideUserCmd = false
			}
		}
	}

	return conf, nil
}

func openAndStripComments(filename string) ([]byte, error) {
	f, err := os.Open(filename)
	if err != nil {
		log.Println(err)
		return nil, err
	}
	defer f.Close()
	var file []byte
	scanner := bufio.NewScanner(f)
scan:
	for scanner.Scan() {
		line := scanner.Bytes()
		foundComment := false
		for i, b := range line {
			if rune(b) == commentSign {
				foundComment = true
			}
			if foundComment {
				if i != 0 {
					file = append(file, byte('\n'))
				}
				continue scan
			} else {
				file = append(file, b)
			}
		}
		file = append(file, byte('\n'))
	}
	err = scanner.Err()
	if err != nil {
		log.Println(err)
		return nil, err
	}
	return file, nil
}

func parseIncludes(file []byte) ([]byte, error) {
	// TODO: this should load and expand every single "include"
	// in the configuration file
	return file, nil
}

func checkForSyntaxErrors(file []byte) error {
	// TODO: check for syntax errors in the file:
	// - Symbols that are not recognized
	// - Options that are unknown
	// - Opened brackets that are not closed (and viceversa)
	return nil
}
