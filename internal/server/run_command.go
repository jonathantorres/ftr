package server

import (
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"os"
	"path"
	"strconv"
	"strings"
)

func runCommandUser(session *Session, username string) error {
	userFound := false
	for _, u := range session.server.Conf.Users {
		if u.Username == username {
			userFound = true
			user := &User{
				Username: u.Username,
			}
			session.user = user
			break
		}
	}
	if userFound {
		return sendResponse(session.controlConn, StatusCodeUsernameOk, "")
	}
	return sendResponse(session.controlConn, StatusCodeInvalidUsername, "")
}

func runCommandPassword(session *Session, pass string) error {
	passFound := false
	for _, u := range session.server.Conf.Users {
		if u.Username == session.user.Username && u.Password == pass {
			passFound = true
			user := &User{
				Username: u.Username,
				Password: u.Password,
				Root:     u.Root,
			}
			session.user = user
			break
		}
	}
	if passFound {
		// change to home directory
		err := os.Chdir(session.server.Conf.Root + session.user.Root)
		if err != nil {
			return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
		}
		return sendResponse(session.controlConn, StatusCodeUserLoggedIn, "")
	}
	return sendResponse(session.controlConn, StatusCodeInvalidUsername, "")
}

func runCommandPrintDir(session *Session) error {
	return sendResponse(session.controlConn, StatusCodePathCreated, " \"/"+session.cwd+"\" is current directory\n")
}

func runCommandChangeDir(session *Session, dir string) error {
	cwd := session.cwd
	if dir[0] == '/' {
		// moving to relative path
		cwd = dir[1:]
	} else {
		if cwd != "" {
			cwd += "/" + dir
		} else {
			cwd = dir
		}
	}
	err := os.Chdir(session.server.Conf.Root + session.user.Root + "/" + cwd)
	if err != nil {
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	session.cwd = cwd
	return sendResponse(session.controlConn, StatusCodeRequestedFileOk, " \"/"+dir+"\" is current directory\n")
}

func runCommandType(session *Session, typ string) error {
	selectedTransferType := TransferType(typ)
	if selectedTransferType == TransferTypeAscii || selectedTransferType == TransferTypeImage {
		session.tType = TransferType(typ)
		return sendResponse(session.controlConn, StatusCodeOk, " Transfer type Ok")
	}
	return sendResponse(session.controlConn, StatusCodeCmdNotImplemented, "")
}

func runCommandPasv(session *Session) error {
	session.passMode = true
	addr, err := session.server.findOpenAddr(false)
	if err != nil {
		return sendResponse(session.controlConn, StatusCodeCantOpenDataConn, "")
	}
	respParts := make([]string, 0)
	for i := 0; i < len(addr.IP); i++ {
		respParts = append(respParts, strconv.Itoa(int(addr.IP[i])))
	}

	var p uint16 = uint16(addr.Port)
	var p1 uint8 = uint8(p >> 8)
	var p2 uint8 = uint8(p)
	respParts = append(respParts, strconv.Itoa(int(p1)))
	respParts = append(respParts, strconv.Itoa(int(p2)))
	respMsg := strings.Join(respParts, ",")

	if err = session.openDataConn(p, false); err != nil {
		return sendResponse(session.controlConn, StatusCodeCantOpenDataConn, "")
	}
	return sendResponse(session.controlConn, StatusCodeEnterPassMode, " "+respMsg)
}

func runCommandList(session *Session, file string) error {
	// wait until the data connection is ready for sending/receiving data
	<-session.dataConnChan

	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd
	if file != "" {
		path += "/" + file
	}
	files, err := ioutil.ReadDir(path)
	if err != nil {
		log.Printf("failed listing directory: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	dirFiles := make([]string, 0)
	for _, f := range files {
		line := getFileLine(f)
		dirFiles = append(dirFiles, line)
	}
	dirData := strings.Join(dirFiles, "\n")
	_, err = session.dataConn.Write([]byte(dirData))
	if err != nil {
		log.Printf("failed writing data: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandFileNames(session *Session, file string) error {
	// wait until the data connection is ready for sending/receiving data
	<-session.dataConnChan

	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd
	if file != "" {
		path += "/" + file
	}
	files, err := ioutil.ReadDir(path)
	if err != nil {
		log.Printf("failed listing directory: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	dirFiles := make([]string, 0, 10)
	for _, f := range files {
		dirFiles = append(dirFiles, f.Name())
	}
	dirData := strings.Join(dirFiles, "\n")
	_, err = session.dataConn.Write([]byte(dirData))
	if err != nil {
		log.Printf("failed writing data: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandRetrieve(session *Session, filename string) error {
	<-session.dataConnChan
	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd + "/" + filename
	file, err := os.Open(path)
	if err != nil {
		log.Printf("error opening file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	defer file.Close()
	_, err = io.Copy(session.dataConn, file)
	if err != nil {
		log.Printf("error transferring file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandAcceptAndStore(session *Session, filename string, appendMode bool) error {
	<-session.dataConnChan
	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd + "/" + filename
	fileData, err := ioutil.ReadAll(session.dataConn)
	if err != nil {
		log.Printf("error receiving file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	var file *os.File
	if appendMode {
		file, err = os.OpenFile(path, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	} else {
		file, err = os.Create(path)
	}
	if err != nil {
		log.Printf("error creating file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	defer file.Close()
	_, err = file.Write(fileData)
	if err != nil {
		log.Printf("error writing bytes to new file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandSystemType(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeNameSystem, " UNIX Type: L8")
}

func runCommandChangeParent(session *Session) error {
	cwd := session.cwd
	pieces := strings.Split(cwd, "/")
	if len(pieces) <= 1 {
		cwd = ""
	} else {
		cwd = strings.Join(pieces[:len(pieces)-1], "/")
	}
	err := os.Chdir(session.server.Conf.Root + session.user.Root + "/" + cwd)
	if err != nil {
		log.Printf("err chdir: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	session.cwd = cwd
	base := path.Base(cwd)

	return sendResponse(session.controlConn, StatusCodeOk, " \"/"+base+"\" is current directory\n")
}

func runCommandMakeDir(session *Session, dirName string) error {
	cwd := session.cwd
	err := os.Mkdir(session.server.Conf.Root+session.user.Root+"/"+cwd+"/"+dirName, 0777)
	if err != nil {
		log.Printf("err mkdir: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, fmt.Sprintf(" Directory %s created", dirName))
}

func runCommandRemoveDir(session *Session, path string) error {
	cwd := session.cwd
	err := os.RemoveAll(session.server.Conf.Root + session.user.Root + "/" + cwd + "/" + path)
	if err != nil {
		log.Printf("error removing directory: %s", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	return sendResponse(session.controlConn, StatusCodeRequestedFileOk, fmt.Sprintf("Directory %s removed", path))
}

func runCommandDelete(session *Session, filename string) error {
	cwd := session.cwd
	err := os.Remove(session.server.Conf.Root + session.user.Root + "/" + cwd + "/" + filename)
	if err != nil {
		log.Printf("err remove file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, fmt.Sprintf(" File %s deleted", filename))
}

func runCommandExtPassMode(session *Session, cmdArgs string) error {
	if cmdArgs != "" {
		if cmdArgs == "1" {
			// only IPv6 allowed
			return sendResponse(session.controlConn, StatusCodeExtPortUnknownProtocol, "")
		}
		return sendResponse(session.controlConn, StatusCodeCmdNotImplemented, "")
	}
	session.passMode = true
	addr, err := session.server.findOpenAddr(true)
	if err != nil {
		log.Printf("error finding an open address: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	p := uint16(addr.Port)
	if err = session.openDataConn(p, true); err != nil {
		log.Printf("error opening data connection: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeCantOpenDataConn, "")
	}
	return sendResponse(session.controlConn, StatusCodeEnterExtPassMode, fmt.Sprintf(" (|||%d|)", p))
}

func runCommandPort(session *Session, cmdArgs string) error {
	// we are ignoring the address here
	// let's just use the port part
	addrParts := strings.Split(cmdArgs, ",")
	portParts := addrParts[4:]
	p1, err := strconv.ParseUint(portParts[0], 10, 8)
	if err != nil {
		log.Printf("error converting port: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	p2, err := strconv.ParseUint(portParts[1], 10, 8)
	if err != nil {
		log.Printf("error converting port: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	p := uint16(p1)
	p <<= 8
	p |= uint16(p2)
	if err = session.connectToDataConn(p, false); err != nil {
		log.Printf("error connecting to data connection: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandExtPort(session *Session, cmdArgs string) error {
	// we are ignoring the address here
	// let's just use the port part
	var useIPv6 bool
	cmdParts := strings.Split(cmdArgs, "|")
	if cmdParts[0] == "2" {
		useIPv6 = true
	}
	p, err := strconv.Atoi(cmdParts[3])
	if err != nil {
		log.Printf("error converting port: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	if err = session.connectToDataConn(uint16(p), useIPv6); err != nil {
		log.Printf("error connecting to data connection: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeUnknownErr, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandHelp(session *Session, cmdArgs string) error {
	var resp bytes.Buffer

	if cmdArgs == "" {
		var welcome = "Welcome to FTR, enter a command name to get more information about it. Current commands: "
		resp.WriteString(welcome)
		resp.WriteString(getAllCommandsHelpMessage())
		resp.WriteString("\n")
	} else {
		h := getCommandHelpMessage(cmdArgs)
		if h == "" {
			resp.WriteString(fmt.Sprintf("Sorry, the command \"%s\" is not implemented\n", cmdArgs))
		} else {
			resp.WriteString(fmt.Sprintf("%s: ", strings.ToUpper(cmdArgs)))
			resp.WriteString(fmt.Sprintf("%s\n", h))
		}
	}
	return sendResponse(session.controlConn, StatusCodeHelpMessage, resp.String())
}

func runCommandNoOp(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandAllo(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandAccount(session *Session, cmdArgs string) error {
	if session.user == nil {
		// no user is logged in
		return sendResponse(session.controlConn, StatusCodeNotLoggedIn, "")
	}
	if cmdArgs == "" {
		return sendResponse(session.controlConn, StatusCodeBadSequence, "")
	}
	if session.user.Username == cmdArgs {
		var resp bytes.Buffer
		resp.WriteString(fmt.Sprintf("Username: %s, Root: %s\n", session.user.Username, session.user.Root))
		return sendResponse(session.controlConn, StatusCodeUserLoggedIn, resp.String())
	}
	return sendResponse(session.controlConn, StatusCodeBadSequence, "The account was not found")
}

func runCommandSite(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeOk, "No SITE options for this server")
}

func runCommandMode(session *Session, cmdArgs string) error {
	if strings.ToLower(cmdArgs) != "s" {
		return sendResponse(session.controlConn, StatusCodeCmdNotImplementedForParam, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandFileStructure(session *Session, cmdArgs string) error {
	if strings.ToLower(cmdArgs) != "f" {
		return sendResponse(session.controlConn, StatusCodeCmdNotImplementedForParam, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandServerStatus(session *Session, cmdArgs string) error {
	if cmdArgs == "" {
		return sendResponse(session.controlConn, StatusCodeSystemStatus, "Server OK")
	}
	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd + "/" + cmdArgs
	files, err := ioutil.ReadDir(path)
	if err != nil {
		log.Printf("failed listing directory: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	dirFiles := make([]string, 0, 10)
	for _, f := range files {
		dirFiles = append(dirFiles, getFileLine(f))
	}
	dirData := strings.Join(dirFiles, "\n")
	return sendResponse(session.controlConn, StatusCodeSystemStatus, dirData)
}

func runUninmplemented(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeCmdNotImplemented, "")
}
