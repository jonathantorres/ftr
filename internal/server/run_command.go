package server

import (
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
	return sendResponse(session.controlConn, StatusCodePathCreated, "\"/"+session.cwd+"\" is current directory\n")
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
	return sendResponse(session.controlConn, StatusCodeRequestedFileOk, "CWD successful. \"/"+dir+"\" is current directory\n")
}

func runCommandType(session *Session, typ string) error {
	selectedTransferType := TransferType(typ)
	if selectedTransferType == TransferTypeAscii || selectedTransferType == TransferTypeImage {
		session.tType = TransferType(typ)
		return sendResponse(session.controlConn, StatusCodeOk, "Transfer type Ok")
	}
	return sendResponse(session.controlConn, StatusCodeCmdNotImplemented, "")
}

func runCommandPasv(session *Session) error {
	session.passMode = true
	addr, err := findOpenAddr()
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

	if err = session.openDataConn(p); err != nil {
		return sendResponse(session.controlConn, StatusCodeCantOpenDataConn, "")
	}
	return sendResponse(session.controlConn, StatusCodeEnterPassMode, respMsg)
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

func runCommandRetrieve(session *Session, filename string) error {
	<-session.dataConnChan
	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd + "/" + filename
	file, err := os.Open(path)
	if err != nil {
		log.Printf("error opening file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	_, err = io.Copy(session.dataConn, file)
	if err != nil {
		log.Printf("error transferring file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	file.Close()
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandAcceptAndStore(session *Session, filename string) error {
	<-session.dataConnChan
	path := session.server.Conf.Root + session.user.Root + "/" + session.cwd + "/" + filename
	fileData, err := ioutil.ReadAll(session.dataConn)
	if err != nil {
		log.Printf("error receiving file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}

	file, err := os.Create(path)
	if err != nil {
		log.Printf("error creating file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	_, err = file.Write(fileData)
	if err != nil {
		log.Printf("error writing bytes to new file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileActionNotTaken, "")
	}
	file.Close()
	var sig struct{}
	session.dataConnChan <- sig
	return sendResponse(session.controlConn, StatusCodeOk, "")
}

func runCommandSystemType(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeNameSystem, "UNIX Type: L8")
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

	return sendResponse(session.controlConn, StatusCodeOk, "CDUP successful. \"/"+base+"\" is current directory\n")
}

func runCommandMakeDir(session *Session, dirName string) error {
	cwd := session.cwd
	err := os.Mkdir(session.server.Conf.Root+session.user.Root+"/"+cwd+"/"+dirName, 0777)
	if err != nil {
		log.Printf("err mkdir: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, fmt.Sprintf("Directory %s created", dirName))
}

func runCommandDelete(session *Session, filename string) error {
	cwd := session.cwd
	err := os.Remove(session.server.Conf.Root + session.user.Root + "/" + cwd + "/" + filename)
	if err != nil {
		log.Printf("err remove file: %s\n", err)
		return sendResponse(session.controlConn, StatusCodeFileNotFound, "")
	}
	return sendResponse(session.controlConn, StatusCodeOk, fmt.Sprintf("File %s deleted", filename))
}

func runUninmplemented(session *Session) error {
	return sendResponse(session.controlConn, StatusCodeCmdNotImplemented, "")
}
