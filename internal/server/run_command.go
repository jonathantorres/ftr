package server

import (
	"bytes"
	"fmt"
	"io"
	"io/ioutil"
	"os"
	"path"
	"strconv"
	"strings"
	"time"
)

const (
	CommandAbort             = "ABOR"
	CommandAccount           = "ACCT"
	CommandAuthData          = "ADAT"
	CommandAllo              = "ALLO"
	CommandAppend            = "APPE"
	CommandAuth              = "AUTH"
	CommandAvail             = "AVBL"
	CommandClear             = "CCC"
	CommandChangeParent      = "CDUP"
	CommandConf              = "CONF"
	CommandCsId              = "CSID"
	CommandChangeDir         = "CWD"
	CommandDelete            = "DELE"
	CommandDirSize           = "DSIZ"
	CommandPrivProtected     = "ENC"
	CommandExtAddrPort       = "EPRT"
	CommandExtPassMode       = "EPSV"
	CommandFeatLis           = "FEAT"
	CommandHelp              = "HELP"
	CommandHost              = "HOST"
	CommandLang              = "LANG"
	CommandList              = "LIST"
	CommandLongAddrPort      = "LPRT"
	CommandLongPassMode      = "LPSV"
	CommandLastModTime       = "MDTM"
	CommandModCreatTime      = "MFCT"
	CommandModFact           = "MFF"
	CommandModLastModTime    = "MFMT"
	CommandInteProtect       = "MIC"
	CommandMakeDir           = "MKD"
	CommandListDir           = "MLSD"
	CommandObjData           = "MLST"
	CommandMode              = "MODE"
	CommandFileNames         = "NLST"
	CommandNoOp              = "NOOP"
	CommandOptions           = "OPTS"
	CommandPassword          = "PASS"
	CommandPassive           = "PASV"
	CommandBufSizeProt       = "PBSZ"
	CommandPort              = "PORT"
	CommandDataChanProtLvl   = "PROT"
	CommandPrintDir          = "PWD"
	CommandQuit              = "QUIT"
	CommandReinit            = "REIN"
	CommandRestart           = "REST"
	CommandRetrieve          = "RETR"
	CommandRemoveDir         = "RMD"
	CommandRemoveDirTree     = "RMDA"
	CommandRenameFrom        = "RNFR"
	CommandRenameTo          = "RNTO"
	CommandSite              = "SITE"
	CommandFileSize          = "SIZE"
	CommandMountFile         = "SMNT"
	CommandSinglePortPassive = "SPSV"
	CommandServerStatus      = "STAT"
	CommandAcceptAndStore    = "STOR"
	CommandStoreFile         = "STOU"
	CommandFileStruct        = "STRU"
	CommandSystemType        = "SYST"
	CommandThumbnail         = "THMB"
	CommandType              = "TYPE"
	CommandUser              = "USER"
	CommandChangeToParentDir = "XCUP"
	CommandMakeADir          = "XMKD"
	CommandPrintCurDir       = "XPWD"
	CommandRemoveTheDir      = "XRMD"
	CommandSendMail          = "XSEM"
	CommandSendTerm          = "XSEN"
)

func runCommandUser(s *Session, username string) error {
	userFound := false
	for _, u := range s.server.Conf.Users {
		if u.Username == username {
			userFound = true
			user := &User{
				Username: u.Username,
			}
			s.user = user
			break
		}
	}

	if userFound {
		return s.server.sendResponse(s.controlConn, StatusCodeUsernameOk, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeInvalidUsername, "")
}

func runCommandPassword(s *Session, pass string) error {
	passFound := false
	for _, u := range s.server.Conf.Users {
		if u.Username == s.user.Username && u.Password == pass {
			passFound = true
			user := &User{
				Username: u.Username,
				Password: u.Password,
				Root:     u.Root,
			}
			s.user = user
			break
		}
	}

	if passFound {
		// change to home directory
		err := os.Chdir(s.server.Conf.Root + s.user.Root)
		if err != nil {
			return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
		}
		return s.server.sendResponse(s.controlConn, StatusCodeUserLoggedIn, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeInvalidUsername, "")
}

func runCommandPrintDir(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodePathCreated, " \"/"+s.cwd+"\" is current directory")
}

func runCommandChangeDir(s *Session, dir string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	cwd := s.cwd
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

	err := os.Chdir(s.server.Conf.Root + s.user.Root + "/" + cwd)
	if err != nil {
		return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
	}

	s.cwd = cwd

	return s.server.sendResponse(s.controlConn, StatusCodeRequestedFileOk, " \"/"+dir+"\" is current directory")
}

func runCommandType(s *Session, typ string) error {
	selectedTransferType := TransferType(typ)
	if selectedTransferType == TransferTypeAscii || selectedTransferType == TransferTypeImage {
		s.tType = TransferType(typ)
		return s.server.sendResponse(s.controlConn, StatusCodeOk, " Transfer type Ok")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeCmdNotImplemented, "")
}

func runCommandPasv(s *Session) error {
	s.passMode = true
	addr, err := s.server.findOpenAddr(false)
	if err != nil {
		return s.server.sendResponse(s.controlConn, StatusCodeCantOpenDataConn, "")
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

	if err = s.openDataConn(p, false); err != nil {
		return s.server.sendResponse(s.controlConn, StatusCodeCantOpenDataConn, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeEnterPassMode, " "+respMsg)
}

func runCommandList(s *Session, file string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	// wait until the data connection is ready for sending/receiving data
	<-s.dataConnChan
	s.transferInProgress = true
	defer func() {
		s.transferInProgress = false
	}()

	path := s.server.Conf.Root + s.user.Root + "/" + s.cwd
	if file != "" {
		path += "/" + file
	}

	files, err := ioutil.ReadDir(path)
	if err != nil {
		s.server.LogErr("failed listing directory: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	dirFiles := make([]string, 0)
	for _, f := range files {
		line := getFileLine(f)
		dirFiles = append(dirFiles, line)
	}

	dirData := strings.Join(dirFiles, "\n")
	_, err = s.dataConn.Write([]byte(dirData))
	if err != nil {
		s.server.LogErr("failed writing data: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	var sig struct{}
	s.dataConnChan <- sig

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandFileNames(s *Session, file string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	// wait until the data connection is ready for sending/receiving data
	<-s.dataConnChan

	s.transferInProgress = true
	defer func() {
		s.transferInProgress = false
	}()

	path := s.server.Conf.Root + s.user.Root + "/" + s.cwd
	if file != "" {
		path += "/" + file
	}

	files, err := ioutil.ReadDir(path)
	if err != nil {
		s.server.LogErr("failed listing directory: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	dirFiles := make([]string, 0, 10)
	for _, f := range files {
		dirFiles = append(dirFiles, f.Name())
	}

	dirData := strings.Join(dirFiles, "\n")
	_, err = s.dataConn.Write([]byte(dirData))
	if err != nil {
		s.server.LogErr("failed writing data: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	var sig struct{}
	s.dataConnChan <- sig

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandRetrieve(s *Session, filename string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	<-s.dataConnChan
	s.transferInProgress = true

	defer func() {
		s.transferInProgress = false
	}()

	path := s.server.Conf.Root + s.user.Root + "/" + s.cwd + "/" + filename
	file, err := os.Open(path)
	if err != nil {
		s.server.LogErr("error opening file: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	defer file.Close()
	_, err = io.Copy(s.dataConn, file)
	if err != nil {
		s.server.LogErr("error transferring file: %s\n", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	var sig struct{}
	s.dataConnChan <- sig

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandAcceptAndStore(s *Session, filename string, appendMode bool) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	<-s.dataConnChan
	s.transferInProgress = true

	defer func() {
		s.transferInProgress = false
	}()

	path := s.server.Conf.Root + s.user.Root + "/" + s.cwd + "/" + filename
	fileData, err := ioutil.ReadAll(s.dataConn)
	if err != nil {
		s.server.LogErr("error receiving file: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	var file *os.File
	if appendMode {
		file, err = os.OpenFile(path, os.O_APPEND|os.O_CREATE|os.O_WRONLY, 0644)
	} else {
		file, err = os.Create(path)
	}

	if err != nil {
		s.server.LogErr("error creating file: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	defer file.Close()
	_, err = file.Write(fileData)
	if err != nil {
		s.server.LogErr("error writing bytes to new file: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	var sig struct{}
	s.dataConnChan <- sig

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandSystemType(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodeNameSystem, " UNIX Type: L8")
}

func runCommandChangeParent(s *Session) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	cwd := s.cwd
	pieces := strings.Split(cwd, "/")
	if len(pieces) <= 1 {
		cwd = ""
	} else {
		cwd = strings.Join(pieces[:len(pieces)-1], "/")
	}

	err := os.Chdir(s.server.Conf.Root + s.user.Root + "/" + cwd)
	if err != nil {
		s.server.LogErr("err chdir: %s\n", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
	}

	s.cwd = cwd
	base := path.Base(cwd)

	return s.server.sendResponse(s.controlConn, StatusCodeOk, " \"/"+base+"\" is current directory")
}

func runCommandMakeDir(s *Session, dirName string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	cwd := s.cwd
	err := os.Mkdir(s.server.Conf.Root+s.user.Root+"/"+cwd+"/"+dirName, 0777)
	if err != nil {
		s.server.LogErr("err mkdir: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, fmt.Sprintf(" Directory %s created", dirName))
}

func runCommandRemoveDir(s *Session, path string) error {
	cwd := s.cwd
	err := os.RemoveAll(s.server.Conf.Root + s.user.Root + "/" + cwd + "/" + path)
	if err != nil {
		s.server.LogErr("error removing directory: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeRequestedFileOk, fmt.Sprintf("Directory %s removed", path))
}

func runCommandDelete(s *Session, filename string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	cwd := s.cwd
	err := os.Remove(s.server.Conf.Root + s.user.Root + "/" + cwd + "/" + filename)
	if err != nil {
		s.server.LogErr("err remove file: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileNotFound, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, fmt.Sprintf(" File %s deleted", filename))
}

func runCommandExtPassMode(s *Session, cmdArgs string) error {
	if cmdArgs != "" {
		if cmdArgs == "1" {
			// only IPv6 allowed
			return s.server.sendResponse(s.controlConn, StatusCodeExtPortUnknownProtocol, "")
		}
		return s.server.sendResponse(s.controlConn, StatusCodeCmdNotImplemented, "")
	}

	s.passMode = true
	addr, err := s.server.findOpenAddr(true)
	if err != nil {
		s.server.LogErr("error finding an open address: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	p := uint16(addr.Port)
	if err = s.openDataConn(p, true); err != nil {
		s.server.LogErr("error opening data connection: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeCantOpenDataConn, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeEnterExtPassMode, fmt.Sprintf(" (|||%d|)", p))
}

func runCommandPort(s *Session, cmdArgs string) error {
	// we are ignoring the address here
	// let's just use the port part
	addrParts := strings.Split(cmdArgs, ",")
	portParts := addrParts[4:]
	p1, err := strconv.ParseUint(portParts[0], 10, 8)
	if err != nil {
		s.server.LogErr("error converting port: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	p2, err := strconv.ParseUint(portParts[1], 10, 8)
	if err != nil {
		s.server.LogErr("error converting port: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	p := uint16(p1)
	p <<= 8
	p |= uint16(p2)

	if err = s.connectToDataConn(p, false); err != nil {
		s.server.LogErr("error connecting to data connection: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandExtPort(s *Session, cmdArgs string) error {
	// we are ignoring the address here
	// let's just use the port part
	var useIPv6 bool
	cmdParts := strings.Split(cmdArgs, "|")
	if cmdParts[0] == "2" {
		useIPv6 = true
	}

	p, err := strconv.Atoi(cmdParts[3])
	if err != nil {
		s.server.LogErr("error converting port: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	if err = s.connectToDataConn(uint16(p), useIPv6); err != nil {
		s.server.LogErr("error connecting to data connection: %s\n", err)
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandHelp(s *Session, cmdArgs string) error {
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

	return s.server.sendResponse(s.controlConn, StatusCodeHelpMessage, resp.String())
}

func runCommandNoOp(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandAllo(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandAccount(s *Session, cmdArgs string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	if cmdArgs == "" {
		return s.server.sendResponse(s.controlConn, StatusCodeBadSequence, "")
	}

	if s.user.Username == cmdArgs {
		var resp bytes.Buffer
		resp.WriteString(fmt.Sprintf("Username: %s, Root: %s\n", s.user.Username, s.user.Root))
		return s.server.sendResponse(s.controlConn, StatusCodeUserLoggedIn, resp.String())
	}

	return s.server.sendResponse(s.controlConn, StatusCodeBadSequence, "The account was not found")
}

func runCommandSite(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodeOk, "No SITE options for this server")
}

func runCommandMode(s *Session, cmdArgs string) error {
	if strings.ToLower(cmdArgs) != "s" {
		return s.server.sendResponse(s.controlConn, StatusCodeCmdNotImplementedForParam, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandFileStructure(s *Session, cmdArgs string) error {
	if strings.ToLower(cmdArgs) != "f" {
		return s.server.sendResponse(s.controlConn, StatusCodeCmdNotImplementedForParam, "")
	}

	return s.server.sendResponse(s.controlConn, StatusCodeOk, "")
}

func runCommandServerStatus(s *Session, cmdArgs string) error {
	if cmdArgs == "" {
		return s.server.sendResponse(s.controlConn, StatusCodeSystemStatus, "Server OK")
	}

	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	path := s.server.Conf.Root + s.user.Root + "/" + s.cwd + "/" + cmdArgs
	files, err := ioutil.ReadDir(path)
	if err != nil {
		s.server.LogErr("failed listing directory: %s", err)
		return s.server.sendResponse(s.controlConn, StatusCodeFileActionNotTaken, "")
	}

	dirFiles := make([]string, 0, 10)
	for _, f := range files {
		dirFiles = append(dirFiles, getFileLine(f))
	}

	dirData := strings.Join(dirFiles, "\n")

	return s.server.sendResponse(s.controlConn, StatusCodeSystemStatus, dirData)
}

func runCommandRenameFrom(s *Session, cmdArgs string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	if cmdArgs == "" {
		return s.server.sendResponse(s.controlConn, StatusCodeSyntaxErr, "A path to rename from is required")
	}

	s.renameFrom = strings.TrimSpace(cmdArgs)

	return s.server.sendResponse(s.controlConn, StatusCodeRequestedFileAction, "")
}

func runCommandRenameTo(s *Session, cmdArgs string) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	if cmdArgs == "" {
		return s.server.sendResponse(s.controlConn, StatusCodeSyntaxErr, "A path to rename to is required")
	}

	if s.renameFrom == "" {
		return s.server.sendResponse(s.controlConn, StatusCodeSyntaxErr, "Path to rename from not provided, please run RNFR first")
	}

	newfile := strings.TrimSpace(cmdArgs)
	oldpath := s.server.Conf.Root + s.user.Root + "/" + s.cwd + "/" + s.renameFrom
	newpath := s.server.Conf.Root + s.user.Root + "/" + s.cwd + "/" + newfile
	err := os.Rename(oldpath, newpath)
	defer func() {
		s.renameFrom = ""
	}()

	if err != nil {
		return s.server.sendResponse(s.controlConn, StatusCodeUnknownErr, err.Error())
	}

	return s.server.sendResponse(s.controlConn, StatusCodeRequestedFileOk, "")
}

func runCommandAbort(s *Session) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	if s.transferInProgress {
		// let's close the data connection channel
		// this will unblock the channel in s.handleDataTransfer()
		// and close the connection and that listener
		close(s.dataConnChan)
		s.transferInProgress = false
		s.server.sendResponse(s.controlConn, StatusCodeConnClosed, "")
		return s.server.sendResponse(s.controlConn, StatusCodeClosingDataConn, "")
	}

	// transfer was not in progress (it must have completed)
	// let's try to close the connection (if it's still open)
	if s.dataConn != nil {
		err := s.dataConn.Close()
		if err != nil {
			s.server.LogErr("error closing the data connection: %s", err)
		}
	}

	return s.server.sendResponse(s.controlConn, StatusCodeClosingDataConn, "")
}

func runCommandReinit(s *Session) error {
	if !s.loggedIn() {
		return s.server.sendResponse(s.controlConn, StatusCodeNotLoggedIn, "")
	}

	// if there is a transfer in progress,
	// let's wait until it's finished
	for {
		if !s.transferInProgress {
			break
		}
		time.Sleep(500 * time.Millisecond)
	}

	s.user = nil
	s.tType = TransferType("")
	s.passMode = false
	s.dataConn = nil
	s.dataConnPort = 0
	s.dataConnChan = nil
	s.cwd = ""
	s.renameFrom = ""
	s.transferInProgress = false

	return s.server.sendResponse(s.controlConn, StatusCodeServiceReady, "")
}

func runCommandQuit(s *Session) error {
	// if there is a transfer in progress,
	// let's wait until it's finished
	if s.loggedIn() {
		for {
			if !s.transferInProgress {
				break
			}
			time.Sleep(500 * time.Millisecond)
		}
	}

	s.server.sendResponse(s.controlConn, StatusCodeClosingControlConn, "")
	s.end()

	return nil
}

func runUninmplemented(s *Session) error {
	return s.server.sendResponse(s.controlConn, StatusCodeCmdNotImplemented, "")
}
