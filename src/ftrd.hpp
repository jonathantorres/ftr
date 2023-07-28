#ifndef ftrd_hpp
#define ftrd_hpp

#include <map>
#include <string>

namespace ftr {
// server constants
const int DEFAULT_CMD_SIZE = 512;
const int CONTROL_PORT = 21;
const int BACKLOG = 4096;

constexpr char const *DEFAULT_CONF = "ftr.conf";
constexpr char const *TRANSFER_TYPE_ASCII = "A";
constexpr char const *TRANSFER_TYPE_IMG = "I";
constexpr char const *DEFAULT_NAME = "localhost";

const int DIR_SIZE = 4096;
const int FILE_BUF = 4096;

// status codes
const int STATUS_CODE_RESTART_MARKER = 110;
const int STATUS_CODE_READY_IN_A_FEW_MINS = 120;
const int STATUS_CODE_DATA_CONN_ALREADY_OPEN = 125;
const int STATUS_CODE_FILE_STATUS_OK = 150;
const int STATUS_CODE_OK = 200;
const int STATUS_CODE_NOT_IMPLEMENTED = 202;
const int STATUS_CODE_SYSTEM_STATUS = 211;
const int STATUS_CODE_DIR_STATUS = 212;
const int STATUS_CODE_FILE_STATUS = 213;
const int STATUS_CODE_HELP_MESSAGE = 214;
const int STATUS_CODE_NAME_SYSTEM = 215;
const int STATUS_CODE_SERVICE_READY = 220;
const int STATUS_CODE_CLOSING_CONTROL_CONN = 221;
const int STATUS_CODE_DATA_CONN_OPEN = 225;
const int STATUS_CODE_CLOSING_DATA_CONN = 226;
const int STATUS_CODE_ENTER_PASS_MODE = 227;
const int STATUS_CODE_ENTER_LONG_PASS_MODE = 228;
const int STATUS_CODE_ENTER_EXT_PASS_MODE = 229;
const int STATUS_CODE_USER_LOGGED_IN = 230;
const int STATUS_CODE_USER_LOGGED_OUT = 231;
const int STATUS_CODE_LOGOUT_CMD_NOTED = 232;
const int STATUS_CODE_AUTH_ACCEPTED = 234;
const int STATUS_CODE_REQUESTED_FILE_OK = 250;
const int STATUS_CODE_PATH_CREATED = 257;
const int STATUS_CODE_USERNAME_OK = 331;
const int STATUS_CODE_NEED_ACCOUNT = 332;
const int STATUS_CODE_REQUESTED_FILE_ACTION = 350;
const int STATUS_CODE_CMD_NOT_ACCEPTED = 400;
const int STATUS_CODE_SERVICE_NOT_AVAILABLE = 421;
const int STATUS_CODE_CANT_OPEN_DATA_CONN = 425;
const int STATUS_CODE_CONN_CLOSED = 426;
const int STATUS_CODE_INVALID_USERNAME = 430;
const int STATUS_CODE_HOST_UNAVAILABLE = 434;
const int STATUS_CODE_FILE_ACTION_NOT_TAKEN = 450;
const int STATUS_CODE_ACTION_ABORTED = 451;
const int STATUS_CODE_ACTION_NOT_TAKEN = 452;
const int STATUS_CODE_UNKNOWN_ERROR = 500;
const int STATUS_CODE_SYNTAX_ERROR = 501;
const int STATUS_CODE_CMD_NOT_IMPLEMENTED = 502;
const int STATUS_CODE_BAD_SEQUENCE = 503;
const int STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM = 504;
const int STATUS_CODE_EXT_PORT_UNKNOWN_PROTOCOL = 522;
const int STATUS_CODE_NOT_LOGGED_IN = 530;
const int STATUS_CODE_NEED_ACCOUNT_FOR_STORING = 532;
const int STATUS_CODE_COULD_NOT_CONN_TO_SERVER = 534;
const int STATUS_CODE_FILE_NOT_FOUND = 550;
const int STATUS_CODE_REQUESTED_ACTION_ABORTED = 551;
const int STATUS_CODE_REQUESTED_FILE_ACTION_ABORTED = 552;
const int STATUS_CODE_REQUESTED_ACTION_NOT_TAKEN = 553;
const int STATUS_CODE_INTEGRITY_PROTECTED_REPLY = 631;
const int STATUS_CODE_CONF_AND_INTEGRITY_PROTECTED_REPLY = 632;
const int STATUS_CODE_CONF_PROTECTED_REPLY = 633;

const std::map<int, std::string> status_codes = {
    {STATUS_CODE_RESTART_MARKER, "Restart marker replay."},
    {STATUS_CODE_READY_IN_A_FEW_MINS, "Service ready in a few minutes."},
    {STATUS_CODE_DATA_CONN_ALREADY_OPEN, "Data connection already open."},
    {STATUS_CODE_FILE_STATUS_OK,
     "File status okay, about to open data connection."},
    {STATUS_CODE_OK, "Command Ok."},
    {STATUS_CODE_NOT_IMPLEMENTED, "Command not implemented."},
    {STATUS_CODE_SYSTEM_STATUS, "System status."},
    {STATUS_CODE_DIR_STATUS, "Directory Status."},
    {STATUS_CODE_FILE_STATUS, "File Status."},
    {STATUS_CODE_HELP_MESSAGE, "Help message."},
    {STATUS_CODE_NAME_SYSTEM, "NAME system type."},
    {STATUS_CODE_SERVICE_READY, "Service Ready."},
    {STATUS_CODE_CLOSING_CONTROL_CONN, "Closing control connection."},
    {STATUS_CODE_DATA_CONN_OPEN,
     "Data connection open, no transfer in progress."},
    {STATUS_CODE_CLOSING_DATA_CONN, "Closing data connection. File action ok."},
    {STATUS_CODE_ENTER_PASS_MODE, "Entering passive mode."},
    {STATUS_CODE_ENTER_LONG_PASS_MODE, "Entering long passive mode."},
    {STATUS_CODE_ENTER_EXT_PASS_MODE, "Entering extended passive mode."},
    {STATUS_CODE_USER_LOGGED_IN,
     "User logged in, proceed. Logged out if appropriate."},
    {STATUS_CODE_USER_LOGGED_OUT, "User logged out, service terminated."},
    {STATUS_CODE_LOGOUT_CMD_NOTED, "Logout command noted."},
    {STATUS_CODE_AUTH_ACCEPTED, "Authentication mechanism accepted."},
    {STATUS_CODE_REQUESTED_FILE_OK, "Requested file action ok, completed."},
    {STATUS_CODE_PATH_CREATED, "Path created."},
    {STATUS_CODE_USERNAME_OK, "Username okay, need password."},
    {STATUS_CODE_NEED_ACCOUNT, "Need account for login."},
    {STATUS_CODE_REQUESTED_FILE_ACTION,
     "Requested file action pending more information."},
    {STATUS_CODE_CMD_NOT_ACCEPTED, "Command not accepted, please try again."},
    {STATUS_CODE_SERVICE_NOT_AVAILABLE,
     "Service not available, closing control connection."},
    {STATUS_CODE_CANT_OPEN_DATA_CONN, "Can't open data connection."},
    {STATUS_CODE_CONN_CLOSED, "Connection closed, transfer aborted."},
    {STATUS_CODE_INVALID_USERNAME, "Invalid username or password."},
    {STATUS_CODE_HOST_UNAVAILABLE, "Requested host unavailable."},
    {STATUS_CODE_FILE_ACTION_NOT_TAKEN, "Requested file action not taken."},
    {STATUS_CODE_ACTION_ABORTED,
     "Requested action aborted. Local error in processing."},
    {STATUS_CODE_ACTION_NOT_TAKEN,
     "Requested action not taken. Insufficient "
     "storage space in system. File unavailable."},
    {STATUS_CODE_UNKNOWN_ERROR, "Unknown error."},
    {STATUS_CODE_SYNTAX_ERROR, "Syntax error in parameters or arguments."},
    {STATUS_CODE_CMD_NOT_IMPLEMENTED, "Command not implemented."},
    {STATUS_CODE_BAD_SEQUENCE, "Bad sequence of commands."},
    {STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM,
     "Command not implemented for that parameter."},
    {STATUS_CODE_EXT_PORT_UNKNOWN_PROTOCOL,
     "Extended Port Failure - unknown network protocol"},
    {STATUS_CODE_NOT_LOGGED_IN, "Not logged in."},
    {STATUS_CODE_NEED_ACCOUNT_FOR_STORING, "Need account for storing files."},
    {STATUS_CODE_COULD_NOT_CONN_TO_SERVER,
     "Could Not Connect to Server - Policy Requires SSL."},
    {STATUS_CODE_FILE_NOT_FOUND, "File not found, error encountered."},
    {STATUS_CODE_REQUESTED_ACTION_ABORTED,
     "Requested action aborted. Page type unknown."},
    {STATUS_CODE_REQUESTED_FILE_ACTION_ABORTED,
     "Requested file action aborted. Exceeded storage allocation."},
    {STATUS_CODE_REQUESTED_ACTION_NOT_TAKEN,
     "Requested action not taken. File name not allowed."},
    {STATUS_CODE_INTEGRITY_PROTECTED_REPLY, "Integrity protected reply."},
    {STATUS_CODE_CONF_AND_INTEGRITY_PROTECTED_REPLY,
     "Confidentiality and integrity protected reply."},
    {STATUS_CODE_CONF_PROTECTED_REPLY, "Confidentiality protected reply."},
};

// commands
constexpr char const *CMD_ABORT = "ABOR";
constexpr char const *CMD_ACCOUNT = "ACCT";
constexpr char const *CMD_AUTH_DATA = "ADAT";
constexpr char const *CMD_ALLO = "ALLO";
constexpr char const *CMD_APPEND = "APPE";
constexpr char const *CMD_AUTH = "AUTH";
constexpr char const *CMD_AVAIL = "AVBL";
constexpr char const *CMD_CLEAR = "CCC";
constexpr char const *CMD_CHANGE_PARENT = "CDUP";
constexpr char const *CMD_CONF = "CONF";
constexpr char const *CMD_CS_ID = "CSID";
constexpr char const *CMD_CHANGE_DIR = "CWD";
constexpr char const *CMD_DELETE = "DELE";
constexpr char const *CMD_DIR_SIZE = "DSIZ";
constexpr char const *CMD_PRIV_PROTECTED = "ENC";
constexpr char const *CMD_EXT_ADDR_PORT = "EPRT";
constexpr char const *CMD_EXT_PASSV_MODE = "EPSV";
constexpr char const *CMD_FEAT_LIST = "FEAT";
constexpr char const *CMD_HELP = "HELP";
constexpr char const *CMD_HOST = "HOST";
constexpr char const *CMD_LANG = "LANG";
constexpr char const *CMD_LIST = "LIST";
constexpr char const *CMD_LONG_ADDR_PORT = "LPRT";
constexpr char const *CMD_LONG_PASSV_MODE = "LPSV";
constexpr char const *CMD_LONG_MOD_TIME = "MDTM";
constexpr char const *CMD_MOD_CREATE_TIME = "MFCT";
constexpr char const *CMD_MOD_FACT = "MFF";
constexpr char const *CMD_MOD_LAST_MOD_TIME = "MFMT";
constexpr char const *CMD_INTE_PROTECT = "MIC";
constexpr char const *CMD_MAKE_DIR = "MKD";
constexpr char const *CMD_LIST_DIR = "MLSD";
constexpr char const *CMD_OBJ_DATA = "MLST";
constexpr char const *CMD_MODE = "MODE";
constexpr char const *CMD_FILE_NAMES = "NLST";
constexpr char const *CMD_NOOP = "NOOP";
constexpr char const *CMD_OPTIONS = "OPTS";
constexpr char const *CMD_PASSWORD = "PASS";
constexpr char const *CMD_PASSIVE = "PASV";
constexpr char const *CMD_BUF_SIZE_PORT = "PBSZ";
constexpr char const *CMD_PORT = "PORT";
constexpr char const *CMD_DATA_CHAN_PROTO_LVL = "PROT";
constexpr char const *CMD_PRINT_DIR = "PWD";
constexpr char const *CMD_QUIT = "QUIT";
constexpr char const *CMD_REINIT = "REIN";
constexpr char const *CMD_RESTART = "REST";
constexpr char const *CMD_RETRIEVE = "RETR";
constexpr char const *CMD_REMOVE_DIR = "RMD";
constexpr char const *CMD_REMOVE_DIR_TREE = "RMDA";
constexpr char const *CMD_RENAME_FROM = "RNFR";
constexpr char const *CMD_RENAME_TO = "RNTO";
constexpr char const *CMD_SITE = "SITE";
constexpr char const *CMD_FILE_SIZE = "SIZE";
constexpr char const *CMD_MOUNT_FILE = "SMNT";
constexpr char const *CMD_SINGLE_PORT_PASSIV = "SPSV";
constexpr char const *CMD_SERVER_STATUS = "STAT";
constexpr char const *CMD_ACCEPT_AND_STORE = "STOR";
constexpr char const *CMD_STORE_FILE = "STOU";
constexpr char const *CMD_FILE_STRUCT = "STRU";
constexpr char const *CMD_SYSTEM_TYPE = "SYST";
constexpr char const *CMD_THUMBNAIL = "THMB";
constexpr char const *CMD_TYPE = "TYPE";
constexpr char const *CMD_USER = "USER";
constexpr char const *CMD_CHANGE_TO_PARENT_DIR = "XCUP";
constexpr char const *CMD_MAKE_A_DIR = "XMKD";
constexpr char const *CMD_PRINT_CUR_DIR = "XPWD";
constexpr char const *CMD_REMOVE_THE_DIR = "XRMD";
constexpr char const *CMD_SEND_MAIL = "XSEM";
constexpr char const *CMD_SEND_TERM = "XSEN";

// help messages
const std::map<std::string, std::string> help_messages = {
    {"abor", "Abort an active file transfer."},
    {"acct", "Account information."},
    {"allo", "Allocate sufficient disk space to receive a file."},
    {"appe", "Append (with create)"},
    {"cdup", "Change to Parent Directory."},
    {"cwd", "Change working directory."},
    {"dele", "Delete file."},
    {"eprt", "Specifies an extended address and port to which the server "
             "should connect."},
    {"epsv", "Enter extended passive mode."},
    {"help", "Returns usage documentation on a command if specified, else a "
             "general help document is returned."},
    {"list", "Returns information of a file or directory if specified, else "
             "information of the current working directory is returned."},
    {"mkd", "Make directory."},
    {"mode", "Sets the transfer mode (Stream, Block, or Compressed)."},
    {"nlst", "Returns a list of file names in a specified directory."},
    {"noop", "No operation (dummy packet; used mostly on keepalives)."},
    {"pass", "Authentication password."},
    {"pasv", "Enter passive mode."},
    {"port",
     "Specifies an address and port to which the server should connect."},
    {"pwd",
     "Print working directory. Returns the current directory of the host."},
    {"quit", "Disconnect."},
    {"rein", "Re initializes the connection."},
    {"retr", "Retrieve a copy of the file"},
    {"rmd", "Remove a directory."},
    {"rnfr", "Rename from."},
    {"rnto", "Rename to."},
    {"site", "Sends site specific commands to remote server"},
    {"smnt", "Mount file structure."},
    {"stat", "Returns information on the server status, including the status "
             "of the current connection"},
    {"stor",
     "Accept the data and to store the data as a file at the server site"},
    {"stou", "Store file uniquely."},
    {"stru", "Set file transfer structure."},
    {"syst", "Return system type."},
    {"type", "Sets the transfer mode (ASCII/Binary)."},
    {"user", "Authentication username."},
    {"xcup", "Change to the parent of the current working directory"},
    {"xmkd", "Make a directory"},
};
} // namespace ftr

#endif
