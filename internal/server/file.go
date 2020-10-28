package server

import (
	"fmt"
	"os"
)

func getFileLine(file os.FileInfo) string {
	mode := file.Mode().String()
	size := file.Size()
	modTime := fmt.Sprintf("%s %d %d", file.ModTime().Month().String(), file.ModTime().Day(), file.ModTime().Year())
	return fmt.Sprintf("%s %d %s %s", mode, size, modTime, file.Name())
}
