# ftr
[![Tests](https://github.com/jonathantorres/ftr/actions/workflows/tests.yml/badge.svg)](https://github.com/jonathantorres/ftr/actions/workflows/tests.yml)

ftr (File Transfer) is an FTP server daemon.

## Installing
Install the binary with `go get`:
```bash
go get -u github.com/jonathantorres/ftr
```

## Build from source
In order to build from source, make sure to use `git` to clone the repository, once done you can use the `make` utility to compile the source code:
```bash
make
```

## Running tests
Use `make` to run all of the tests:
```bash
make test
```

## Configuration
You can see an example configuration file in `ftr.conf`, in here you can customize the users for the server along with their passwords and root directories. Every configuration option is explained below.

- `server`: Specify an IP address or domain name in which the server will listen to requests from clients
- `port`: Port in which to run the server
- `root`: Root directory in which all of the directories for it's users will be stored
- `error_log`: Location in which log entries related to errors on the server will be stored
- `access_log`: Location in which log entries related to normal operations of the server will be stored
- `user`: This option will allow you to add a user for the server
- `user.username`: Specify a username for the user
- `user.password`: Specify a password for the user
- `user.root`: Specify a root directory for the user. This directory will be relative to the location of the `root` of the server

## Command Line options
You can use the `-h` flag to see all of the command line options in which the server can run
```bash
ftr -h
```
