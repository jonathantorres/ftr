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
	331: "User name okay, need password.",
	500: "Unknown error",
}

func GetStatusCodeMessage(statusCode uint16) (string, error) {
	for code, statusMsg := range statusCodes {
		if code == statusCode {
			return statusMsg, nil
		}
	}
	return "", errors.New("status code not found")
}