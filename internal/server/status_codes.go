package server

import (
	"bytes"
	"fmt"
	"strings"
)

const (
	StatusCodeRestartMarker       = 110
	StatusServiceReadyInAFewMins  = 120
	StatusCodeDataConnAlreadyOpen = 125
	StatusCodeFileStatusOk        = 150

	StatusCodeOk                 = 200
	StatusCodeNotImplemented     = 202
	StatusCodeSystemStatus       = 211
	StatusCodeDirectoryStatus    = 212
	StatusCodeFileStatus         = 213
	StatusCodeHelpMessage        = 214
	StatusCodeNameSystem         = 215
	StatusCodeServiceReady       = 220
	StatusCodeClosingControlConn = 221
	StatusCodeDataConnOpen       = 225
	StatusCodeClosingDataConn    = 226
	StatusCodeEnterPassMode      = 227
	StatusCodeEnterLongPassMode  = 228
	StatusCodeEnterExtPassMode   = 229
	StatusCodeUserLoggedIn       = 230
	StatusCodeUserLoggedOut      = 231
	StatusCodeLogoutCmdNoted     = 232
	StatusCodeAuthAccepted       = 234
	StatusCodeRequestedFileOk    = 250
	StatusCodePathCreated        = 257

	StatusCodeUsernameOk          = 331
	StatusCodeNeedAccount         = 332
	StatusCodeRequestedFileAction = 350

	StatusCodeCmdNotAccepted      = 400
	StatusCodeServiceNotAvailable = 421
	StatusCodeCantOpenDataConn    = 425
	StatusCodeConnClosed          = 426
	StatusCodeInvalidUsername     = 430
	StatusCodeHostUnavailable     = 434
	StatusCodeFileActionNotTaken  = 450
	StatusCodeActionAborted       = 451
	StatusCodeActionNotTaken      = 452

	StatusCodeUnknownErr                 = 500
	StatusCodeSyntaxErr                  = 501
	StatusCodeCmdNotImplemented          = 502
	StatusCodeBadSequence                = 503
	StatusCodeCmdNotImplementedForParam  = 504
	StatusCodeExtPortUnknownProtocol     = 522
	StatusCodeNotLoggedIn                = 530
	StatusCodeNeedAccountForStoring      = 532
	StatusCodeCouldNotConnToServer       = 534
	StatusCodeFileNotFound               = 550
	StatusCodeRequestedActionAborted     = 551
	StatusCodeRequestedFileActionAborted = 552
	StatusCodeRequestedActionNotTaken    = 553

	StatusCodeIntegrityProtectedReply        = 631
	StatusCodeConfAndIntegrityProtectedReply = 632
	StatusCodeConfProtectedReply             = 633
)

var statusCodes = map[uint16]string{
	StatusCodeRestartMarker:       "Restart marker replay.",
	StatusServiceReadyInAFewMins:  "Service ready in a few minutes.",
	StatusCodeDataConnAlreadyOpen: "Data connection already open.",
	StatusCodeFileStatusOk:        "File status okay, about to open data connection.",

	StatusCodeOk:                 "Command Ok.",
	StatusCodeNotImplemented:     "Command not implemented.",
	StatusCodeSystemStatus:       "System status.",
	StatusCodeDirectoryStatus:    "Directory Status.",
	StatusCodeFileStatus:         "File Status.",
	StatusCodeHelpMessage:        "Help message.",
	StatusCodeNameSystem:         "NAME system type.",
	StatusCodeServiceReady:       "Service Ready.",
	StatusCodeClosingControlConn: "Closing control connection.",
	StatusCodeDataConnOpen:       "Data connection open, no transfer in progress.",
	StatusCodeClosingDataConn:    "Closing data connection. File action ok.",
	StatusCodeEnterPassMode:      "Entering passive mode.",
	StatusCodeEnterLongPassMode:  "Entering long passive mode.",
	StatusCodeEnterExtPassMode:   "Entering extended passive mode.",
	StatusCodeUserLoggedIn:       "User logged in, proceed. Logged out if appropriate.",
	StatusCodeUserLoggedOut:      "User logged out, service terminated.",
	StatusCodeLogoutCmdNoted:     "Logout command noted.",
	StatusCodeAuthAccepted:       "Authentication mechanism accepted.",
	StatusCodeRequestedFileOk:    "Requested file action ok, completed.",
	StatusCodePathCreated:        "Path created.",

	StatusCodeUsernameOk:          "Username okay, need password.",
	StatusCodeNeedAccount:         "Need account for login.",
	StatusCodeRequestedFileAction: "Requested file action pending more information.",

	StatusCodeCmdNotAccepted:      "Command not accepted, please try again.",
	StatusCodeServiceNotAvailable: "Service not available, closing control connection.",
	StatusCodeCantOpenDataConn:    "Can't open data connection.",
	StatusCodeConnClosed:          "Connection closed, transfer aborted.",
	StatusCodeInvalidUsername:     "Invalid username or password.",
	StatusCodeHostUnavailable:     "Requested host unavailable.",
	StatusCodeFileActionNotTaken:  "Requested file action not taken.",
	StatusCodeActionAborted:       "Requested action aborted. Local error in processing.",
	StatusCodeActionNotTaken:      "Requested action not taken. Insufficient storage space in system. File unavailable.",

	StatusCodeUnknownErr:                 "Unknown error.",
	StatusCodeSyntaxErr:                  "Syntax error in parameters or arguments.",
	StatusCodeCmdNotImplemented:          "Command not implemented.",
	StatusCodeBadSequence:                "Bad sequence of commands.",
	StatusCodeCmdNotImplementedForParam:  "Command not implemented for that parameter.",
	StatusCodeExtPortUnknownProtocol:     "Extended Port Failure - unknown network protocol",
	StatusCodeNotLoggedIn:                "Not logged in.",
	StatusCodeNeedAccountForStoring:      "Need account for storing files.",
	StatusCodeCouldNotConnToServer:       "Could Not Connect to Server - Policy Requires SSL.",
	StatusCodeFileNotFound:               "File not found, error encountered.",
	StatusCodeRequestedActionAborted:     "Requested action aborted. Page type unknown.",
	StatusCodeRequestedFileActionAborted: "Requested file action aborted. Exceeded storage allocation.",
	StatusCodeRequestedActionNotTaken:    "Requested action not taken. File name not allowed.",

	StatusCodeIntegrityProtectedReply:        "Integrity protected reply.",
	StatusCodeConfAndIntegrityProtectedReply: "Confidentiality and integrity protected reply.",
	StatusCodeConfProtectedReply:             "Confidentiality protected reply.",
}

func GetStatusCodeMessage(statusCode uint16) string {
	for code, statusMsg := range statusCodes {
		if code == statusCode {
			return statusMsg
		}
	}
	return ""
}

var helpMessages = map[string]string{
	"abor": "Abort an active file transfer.",
	"acct": "Account information.",
	"allo": "Allocate sufficient disk space to receive a file.",
	"appe": "Append (with create)",
	"cdup": "Change to Parent Directory.",
	"cwd":  "Change working directory.",
	"dele": "Delete file.",
	"eprt": "Specifies an extended address and port to which the server should connect.",
	"epsv": "Enter extended passive mode.",
	"feat": "Get the feature list implemented by the server.",
	"help": "Returns usage documentation on a command if specified, else a general help document is returned.",
	"list": "Returns information of a file or directory if specified, else information of the current working directory is returned.",
	"mkd":  "Make directory.",
	"mode": "Sets the transfer mode (Stream, Block, or Compressed).",
	"nlst": "Returns a list of file names in a specified directory.",
	"noop": "No operation (dummy packet; used mostly on keepalives).",
	"pass": "Authentication password.",
	"pasv": "Enter passive mode.",
	"port": "Specifies an address and port to which the server should connect.",
	"pwd":  "Print working directory. Returns the current directory of the host.",
	"quit": "Disconnect.",
	"rein": "Re initializes the connection.",
	"retr": "Retrieve a copy of the file",
	"rmd":  "Remove a directory.",
	"rnfr": "Rename from.",
	"rnto": "Rename to.",
	"site": "Sends site specific commands to remote server",
	"smnt": "Mount file structure.",
	"stat": "Returns information on the server status, including the status of the current connection",
	"stor": "Accept the data and to store the data as a file at the server site",
	"stou": "Store file uniquely.",
	"stru": "Set file transfer structure.",
	"syst": "Return system type.",
	"type": "Sets the transfer mode (ASCII/Binary).",
	"user": "Authentication username.",
	"xcup": "Change to the parent of the current working directory",
	"xmkd": "Make a directory",
}

func getCommandHelpMessage(cmd string) string {
	c, ok := helpMessages[strings.ToLower(cmd)]
	if ok {
		return c
	}
	return ""
}

func getAllCommandsHelpMessage() string {
	var b bytes.Buffer
	for cmd := range helpMessages {
		b.WriteString(fmt.Sprintf("%s ", strings.ToUpper(cmd)))
	}
	return b.String()
}
