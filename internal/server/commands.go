package server

import (
	"os"
	"path"
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

func runCommandUser(session *Session, username string) error {
	userFound := false
	for _, u := range session.server.Conf.Users {
		if u.Username == username {
			userFound = true
			session.user = u
			break
		}
	}
	if userFound {
		return sendResponse(session.controlConn, 331, "")
	}
	return sendResponse(session.controlConn, 430, "")
}

func runCommandPassword(session *Session, pass string) error {
	passFound := false
	for _, u := range session.server.Conf.Users {
		if u.Username == session.user.Username && u.Password == pass {
			passFound = true
			session.user = u
			break
		}
	}
	if passFound {
		return sendResponse(session.controlConn, 230, "")
	}
	return sendResponse(session.controlConn, 430, "")
}

func runCommandPrintDir(session *Session) error {
	basename := path.Base(session.cwd)
	err := os.Chdir(session.server.Conf.Root + session.user.Root + session.cwd)
	if err != nil {
		return sendResponse(session.controlConn, 550, "")
	}
	return sendResponse(session.controlConn, 257, "\"/"+basename+"\" is current directory\n")
}

func runCommandChangeDir(session *Session, dir string) error {
	basename := path.Base(session.cwd + dir)
	err := os.Chdir(session.server.Conf.Root + session.user.Root + dir)
	if err != nil {
		return sendResponse(session.controlConn, 550, "")
	}
	session.cwd = dir
	return sendResponse(session.controlConn, 250, "CWD successful. \"/"+basename+"\" is current directory\n")
}

func runCommandType(session *Session, typ string) error {
	// TODO: logic to set the transfer type
	return sendResponse(session.controlConn, 200, "")
}

func runCommandPasv(session *Session) error {
	// TODO: logic to set passive mode
	return sendResponse(session.controlConn, 200, "")
}

func runCommandPort(session *Session, port string) error {
	// TODO: logic open data conn with the port
	return sendResponse(session.controlConn, 200, "")
}

func runCommandList(session *Session) error {
	// TODO: logic to list the current working directory
	return sendResponse(session.controlConn, 200, "")
}

func runUninmplemented(session *Session) error {
	return sendResponse(session.controlConn, 502, "")
}
