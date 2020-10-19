package server

import (
	"errors"
)

// type struct StatusCode{
// 	uint16 code
// 	string message
// }

var statusCodes = map[uint16]string{
	220: "Service Ready",
	230: "User logged in, proceed.",
	331: "Username okay, need password.",
	500: "Unknown error",
	502: "Command not implemented",
}

func GetStatusCodeMessage(statusCode uint16) (string, error) {
	for code, statusMsg := range statusCodes {
		if code == statusCode {
			return statusMsg, nil
		}
	}
	return "", errors.New("status code not found")
}
