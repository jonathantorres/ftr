package server

import (
	"errors"
)

// type struct StatusCode{
// 	uint16 code
// 	string message
// }

var statusCodes = map[uint16]string{
	200: "Command Ok",
	202: "Command not implemented",
	211: "System status",
	212: "Directory Status",
	213: "File Status",
	214: "Help message",
	215: "NAME system type",
	220: "Service Ready",
	221: "Closing control connection",
	225: "Data connection open, no transfer in progress",
	226: "Closing data connection. File action ok",
	227: "Entering passive mode",
	228: "Entering long passive mode",
	229: "Entering extended passive mode",
	230: "User logged in, proceed. Logged out if appropriate",
	231: "User logged out, service terminated.",
	232: "Logout command noted",
	234: "Authentication mechanism accepted",
	250: "Requested file action ok, completed",
	257: "Path created",

	331: "Username okay, need password.",

	400: "Command not accepted, please try again",
	421: "Service not available, closing control connection",
	425: "Can't open data connection",
	426: "Connection closed, transfer aborted",
	430: "Invalid username or password",
	434: "Requested host unavailable",
	450: "Requested file action not taken",
	451: "Requested action aborted. Local error in processing",
	452: "Requested action not taken. Insufficient storage space in system. File unavailable",

	500: "Unknown error",
	501: "Syntax error in parameters or arguments. ",
	502: "Command not implemented",
	503: "Bad sequence of commands",
	504: "Command not implemented for that parameter.",
	530: "Not logged in.",
	532: "Need account for storing files.",
	534: "Could Not Connect to Server - Policy Requires SSL",
	550: "File not found, error encountered",
	551: "Requested action aborted. Page type unknown.",
	552: "Requested file action aborted. Exceeded storage allocation",
	553: "equested action not taken. File name not allowed.",
}

func GetStatusCodeMessage(statusCode uint16) (string, error) {
	for code, statusMsg := range statusCodes {
		if code == statusCode {
			return statusMsg, nil
		}
	}
	return "", errors.New("status code not found")
}
