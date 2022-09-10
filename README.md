# ftr
[![Tests](https://github.com/jonathantorres/ftr/actions/workflows/tests.yml/badge.svg)](https://github.com/jonathantorres/ftr/actions/workflows/tests.yml)

FTR (File Transfer) is an FTP server.

## Installing/Building from source
In order to build a release from source, make sure to use `git` to clone the repository, once done you can use `cmake` and `make` to compile the source code. We recommend using the `CMAKE_INSTALL_PREFIX` to select the location in which to install, the suggested location is `/usr/local/ftr`.
```bash
cmake -S . -B release -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr/local/ftr
cd release
make ftr
sudo make install
```

## Development build and running tests
Use `cmake` to build the development version, for code changes and bugfixes, and also to build and run all of the tests.
```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX={your git clone location}
cd build
cmake --build .
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
You can use the `-h` option to see all of the command line options in which the server can run
```bash
ftr -h
```
