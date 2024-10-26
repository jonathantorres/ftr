package server

import (
	"fmt"
	"os"
	"os/user"
	"strconv"
	"strings"
	"syscall"
)

const defaultDirSize = 4096

func getFileLine(file os.FileInfo) string {
	var line strings.Builder

	// file mode bits
	line.WriteString(file.Mode().String())

	// owner and group name
	line.WriteString(" " + getOwnerName(file))
	line.WriteString(" " + getGroupName(file))

	// file size
	if file.IsDir() {
		line.WriteString(fmt.Sprintf(" %d", defaultDirSize))
	} else {
		line.WriteString(fmt.Sprintf(" %d", file.Size()))
	}

	// modification time
	line.WriteString(" " + file.ModTime().Format("Jan 01 15:05"))

	// file name
	line.WriteString(" " + file.Name())

	return line.String()
}

func getGroupName(file os.FileInfo) string {
	groupName := "unknown"
	stat, ok := file.Sys().(*syscall.Stat_t)

	if ok {
		gId := strconv.FormatUint(uint64(stat.Gid), 10)
		group, err := user.LookupGroupId(gId)

		if err != nil {
			return groupName
		}

		groupName = group.Name
	}

	return groupName
}

func getOwnerName(file os.FileInfo) string {
	ownerName := "unknown"
	stat, ok := file.Sys().(*syscall.Stat_t)

	if ok {
		uId := strconv.FormatUint(uint64(stat.Uid), 10)
		usr, err := user.LookupId(uId)

		if err != nil {
			return ownerName
		}

		ownerName = usr.Username
	}

	return ownerName
}
