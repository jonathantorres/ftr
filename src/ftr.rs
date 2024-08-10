pub use conf::Conf;
pub use log::Log;

pub const VERSION: &str = "0.0.1";
pub const PROG_NAME: &str = "ftr";

// server constants
const DEFAULT_CMD_SIZE: i32 = 512;
const CONTROL_PORT: i32 = 21;
const BACKLOG: i32 = 4096;

pub const DEFAULT_CONF: &str = "ftrd.conf";
const TRANSFER_TYPE_ASCII: &str = "A";
const TRANSFER_TYPE_IMG: &str = "I";
const DEFAULT_NAME: &str = "localhost";

// file related constants
const DIR_SIZE: i32 = 4096;
const FILE_BUF: i32 = 4096;

// status codes
const STATUS_CODE_RESTART_MARKER: i32 = 110;
const STATUS_CODE_READY_IN_A_FEW_MINS: i32 = 120;
const STATUS_CODE_DATA_CONN_ALREADY_OPEN: i32 = 125;
const STATUS_CODE_FILE_STATUS_OK: i32 = 150;
const STATUS_CODE_OK: i32 = 200;
const STATUS_CODE_NOT_IMPLEMENTED: i32 = 202;
const STATUS_CODE_SYSTEM_STATUS: i32 = 211;
const STATUS_CODE_DIR_STATUS: i32 = 212;
const STATUS_CODE_FILE_STATUS: i32 = 213;
const STATUS_CODE_HELP_MESSAGE: i32 = 214;
const STATUS_CODE_NAME_SYSTEM: i32 = 215;
const STATUS_CODE_SERVICE_READY: i32 = 220;
const STATUS_CODE_CLOSING_CONTROL_CONN: i32 = 221;
const STATUS_CODE_DATA_CONN_OPEN: i32 = 225;
const STATUS_CODE_CLOSING_DATA_CONN: i32 = 226;
const STATUS_CODE_ENTER_PASS_MODE: i32 = 227;
const STATUS_CODE_ENTER_LONG_PASS_MODE: i32 = 228;
const STATUS_CODE_ENTER_EXT_PASS_MODE: i32 = 229;
const STATUS_CODE_USER_LOGGED_IN: i32 = 230;
const STATUS_CODE_USER_LOGGED_OUT: i32 = 231;
const STATUS_CODE_LOGOUT_CMD_NOTED: i32 = 232;
const STATUS_CODE_AUTH_ACCEPTED: i32 = 234;
const STATUS_CODE_REQUESTED_FILE_OK: i32 = 250;
const STATUS_CODE_PATH_CREATED: i32 = 257;
const STATUS_CODE_USERNAME_OK: i32 = 331;
const STATUS_CODE_NEED_ACCOUNT: i32 = 332;
const STATUS_CODE_REQUESTED_FILE_ACTION: i32 = 350;
const STATUS_CODE_CMD_NOT_ACCEPTED: i32 = 400;
const STATUS_CODE_SERVICE_NOT_AVAILABLE: i32 = 421;
const STATUS_CODE_CANT_OPEN_DATA_CONN: i32 = 425;
const STATUS_CODE_CONN_CLOSED: i32 = 426;
const STATUS_CODE_INVALID_USERNAME: i32 = 430;
const STATUS_CODE_HOST_UNAVAILABLE: i32 = 434;
const STATUS_CODE_FILE_ACTION_NOT_TAKEN: i32 = 450;
const STATUS_CODE_ACTION_ABORTED: i32 = 451;
const STATUS_CODE_ACTION_NOT_TAKEN: i32 = 452;
const STATUS_CODE_UNKNOWN_ERROR: i32 = 500;
const STATUS_CODE_SYNTAX_ERROR: i32 = 501;
const STATUS_CODE_CMD_NOT_IMPLEMENTED: i32 = 502;
const STATUS_CODE_BAD_SEQUENCE: i32 = 503;
const STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM: i32 = 504;
const STATUS_CODE_EXT_PORT_UNKNOWN_PROTOCOL: i32 = 522;
const STATUS_CODE_NOT_LOGGED_IN: i32 = 530;
const STATUS_CODE_NEED_ACCOUNT_FOR_STORING: i32 = 532;
const STATUS_CODE_COULD_NOT_CONN_TO_SERVER: i32 = 534;
const STATUS_CODE_FILE_NOT_FOUND: i32 = 550;
const STATUS_CODE_REQUESTED_ACTION_ABORTED: i32 = 551;
const STATUS_CODE_REQUESTED_FILE_ACTION_ABORTED: i32 = 552;
const STATUS_CODE_REQUESTED_ACTION_NOT_TAKEN: i32 = 553;
const STATUS_CODE_INTEGRITY_PROTECTED_REPLY: i32 = 631;
const STATUS_CODE_CONF_AND_INTEGRITY_PROTECTED_REPLY: i32 = 632;
const STATUS_CODE_CONF_PROTECTED_REPLY: i32 = 633;

// figure out how to create the status codes HashMap
// it should probably live on the Heap and be there for
// the duration of the program
// const std::map<int, std::string> status_codes = {
//     {STATUS_CODE_RESTART_MARKER, "Restart marker replay."},
//     {STATUS_CODE_READY_IN_A_FEW_MINS, "Service ready in a few minutes."},
//     {STATUS_CODE_DATA_CONN_ALREADY_OPEN, "Data connection already open."},
//     {STATUS_CODE_FILE_STATUS_OK,
//      "File status okay, about to open data connection."},
//     {STATUS_CODE_OK, "Command Ok."},
//     {STATUS_CODE_NOT_IMPLEMENTED, "Command not implemented."},
//     {STATUS_CODE_SYSTEM_STATUS, "System status."},
//     {STATUS_CODE_DIR_STATUS, "Directory Status."},
//     {STATUS_CODE_FILE_STATUS, "File Status."},
//     {STATUS_CODE_HELP_MESSAGE, "Help message."},
//     {STATUS_CODE_NAME_SYSTEM, "NAME system type."},
//     {STATUS_CODE_SERVICE_READY, "Service Ready."},
//     {STATUS_CODE_CLOSING_CONTROL_CONN, "Closing control connection."},
//     {STATUS_CODE_DATA_CONN_OPEN,
//      "Data connection open, no transfer in progress."},
//     {STATUS_CODE_CLOSING_DATA_CONN, "Closing data connection. File action ok."},
//     {STATUS_CODE_ENTER_PASS_MODE, "Entering passive mode."},
//     {STATUS_CODE_ENTER_LONG_PASS_MODE, "Entering long passive mode."},
//     {STATUS_CODE_ENTER_EXT_PASS_MODE, "Entering extended passive mode."},
//     {STATUS_CODE_USER_LOGGED_IN,
//      "User logged in, proceed. Logged out if appropriate."},
//     {STATUS_CODE_USER_LOGGED_OUT, "User logged out, service terminated."},
//     {STATUS_CODE_LOGOUT_CMD_NOTED, "Logout command noted."},
//     {STATUS_CODE_AUTH_ACCEPTED, "Authentication mechanism accepted."},
//     {STATUS_CODE_REQUESTED_FILE_OK, "Requested file action ok, completed."},
//     {STATUS_CODE_PATH_CREATED, "Path created."},
//     {STATUS_CODE_USERNAME_OK, "Username okay, need password."},
//     {STATUS_CODE_NEED_ACCOUNT, "Need account for login."},
//     {STATUS_CODE_REQUESTED_FILE_ACTION,
//      "Requested file action pending more information."},
//     {STATUS_CODE_CMD_NOT_ACCEPTED, "Command not accepted, please try again."},
//     {STATUS_CODE_SERVICE_NOT_AVAILABLE,
//      "Service not available, closing control connection."},
//     {STATUS_CODE_CANT_OPEN_DATA_CONN, "Can't open data connection."},
//     {STATUS_CODE_CONN_CLOSED, "Connection closed, transfer aborted."},
//     {STATUS_CODE_INVALID_USERNAME, "Invalid username or password."},
//     {STATUS_CODE_HOST_UNAVAILABLE, "Requested host unavailable."},
//     {STATUS_CODE_FILE_ACTION_NOT_TAKEN, "Requested file action not taken."},
//     {STATUS_CODE_ACTION_ABORTED,
//      "Requested action aborted. Local error in processing."},
//     {STATUS_CODE_ACTION_NOT_TAKEN,
//      "Requested action not taken. Insufficient "
//      "storage space in system. File unavailable."},
//     {STATUS_CODE_UNKNOWN_ERROR, "Unknown error."},
//     {STATUS_CODE_SYNTAX_ERROR, "Syntax error in parameters or arguments."},
//     {STATUS_CODE_CMD_NOT_IMPLEMENTED, "Command not implemented."},
//     {STATUS_CODE_BAD_SEQUENCE, "Bad sequence of commands."},
//     {STATUS_CODE_CMD_NOT_IMPLEMENTED_FOR_PARAM,
//      "Command not implemented for that parameter."},
//     {STATUS_CODE_EXT_PORT_UNKNOWN_PROTOCOL,
//      "Extended Port Failure - unknown network protocol"},
//     {STATUS_CODE_NOT_LOGGED_IN, "Not logged in."},
//     {STATUS_CODE_NEED_ACCOUNT_FOR_STORING, "Need account for storing files."},
//     {STATUS_CODE_COULD_NOT_CONN_TO_SERVER,
//      "Could Not Connect to Server - Policy Requires SSL."},
//     {STATUS_CODE_FILE_NOT_FOUND, "File not found, error encountered."},
//     {STATUS_CODE_REQUESTED_ACTION_ABORTED,
//      "Requested action aborted. Page type unknown."},
//     {STATUS_CODE_REQUESTED_FILE_ACTION_ABORTED,
//      "Requested file action aborted. Exceeded storage allocation."},
//     {STATUS_CODE_REQUESTED_ACTION_NOT_TAKEN,
//      "Requested action not taken. File name not allowed."},
//     {STATUS_CODE_INTEGRITY_PROTECTED_REPLY, "Integrity protected reply."},
//     {STATUS_CODE_CONF_AND_INTEGRITY_PROTECTED_REPLY,
//      "Confidentiality and integrity protected reply."},
//     {STATUS_CODE_CONF_PROTECTED_REPLY, "Confidentiality protected reply."},
// };

// commands
const CMD_ABORT: &str = "ABOR";
const CMD_ACCOUNT: &str = "ACCT";
const CMD_AUTH_DATA: &str = "ADAT";
const CMD_ALLO: &str = "ALLO";
const CMD_APPEND: &str = "APPE";
const CMD_AUTH: &str = "AUTH";
const CMD_AVAIL: &str = "AVBL";
const CMD_CLEAR: &str = "CCC";
const CMD_CHANGE_PARENT: &str = "CDUP";
const CMD_CONF: &str = "CONF";
const CMD_CS_ID: &str = "CSID";
const CMD_CHANGE_DIR: &str = "CWD";
const CMD_DELETE: &str = "DELE";
const CMD_DIR_SIZE: &str = "DSIZ";
const CMD_PRIV_PROTECTED: &str = "ENC";
const CMD_EXT_ADDR_PORT: &str = "EPRT";
const CMD_EXT_PASSV_MODE: &str = "EPSV";
const CMD_FEAT_LIST: &str = "FEAT";
const CMD_HELP: &str = "HELP";
const CMD_HOST: &str = "HOST";
const CMD_LANG: &str = "LANG";
const CMD_LIST: &str = "LIST";
const CMD_LONG_ADDR_PORT: &str = "LPRT";
const CMD_LONG_PASSV_MODE: &str = "LPSV";
const CMD_LONG_MOD_TIME: &str = "MDTM";
const CMD_MOD_CREATE_TIME: &str = "MFCT";
const CMD_MOD_FACT: &str = "MFF";
const CMD_MOD_LAST_MOD_TIME: &str = "MFMT";
const CMD_INTE_PROTECT: &str = "MIC";
const CMD_MAKE_DIR: &str = "MKD";
const CMD_LIST_DIR: &str = "MLSD";
const CMD_OBJ_DATA: &str = "MLST";
const CMD_MODE: &str = "MODE";
const CMD_FILE_NAMES: &str = "NLST";
const CMD_NOOP: &str = "NOOP";
const CMD_OPTIONS: &str = "OPTS";
const CMD_PASSWORD: &str = "PASS";
const CMD_PASSIVE: &str = "PASV";
const CMD_BUF_SIZE_PORT: &str = "PBSZ";
const CMD_PORT: &str = "PORT";
const CMD_DATA_CHAN_PROTO_LVL: &str = "PROT";
const CMD_PRINT_DIR: &str = "PWD";
const CMD_QUIT: &str = "QUIT";
const CMD_REINIT: &str = "REIN";
const CMD_RESTART: &str = "REST";
const CMD_RETRIEVE: &str = "RETR";
const CMD_REMOVE_DIR: &str = "RMD";
const CMD_REMOVE_DIR_TREE: &str = "RMDA";
const CMD_RENAME_FROM: &str = "RNFR";
const CMD_RENAME_TO: &str = "RNTO";
const CMD_SITE: &str = "SITE";
const CMD_FILE_SIZE: &str = "SIZE";
const CMD_MOUNT_FILE: &str = "SMNT";
const CMD_SINGLE_PORT_PASSIV: &str = "SPSV";
const CMD_SERVER_STATUS: &str = "STAT";
const CMD_ACCEPT_AND_STORE: &str = "STOR";
const CMD_STORE_FILE: &str = "STOU";
const CMD_FILE_STRUCT: &str = "STRU";
const CMD_SYSTEM_TYPE: &str = "SYST";
const CMD_THUMBNAIL: &str = "THMB";
const CMD_TYPE: &str = "TYPE";
const CMD_USER: &str = "USER";
const CMD_CHANGE_TO_PARENT_DIR: &str = "XCUP";
const CMD_MAKE_A_DIR: &str = "XMKD";
const CMD_PRINT_CUR_DIR: &str = "XPWD";
const CMD_REMOVE_THE_DIR: &str = "XRMD";
const CMD_SEND_MAIL: &str = "XSEM";
const CMD_SEND_TERM: &str = "XSEN";

// figure out how to create the help messages HashMap
// it should probably live on the Heap and be there for
// the duration of the program
// const std::map<std::string, std::string> help_messages = {
//     {"abor", "Abort an active file transfer."},
//     {"acct", "Account information."},
//     {"allo", "Allocate sufficient disk space to receive a file."},
//     {"appe", "Append (with create)"},
//     {"cdup", "Change to Parent Directory."},
//     {"cwd", "Change working directory."},
//     {"dele", "Delete file."},
//     {"eprt", "Specifies an extended address and port to which the server "
//              "should connect."},
//     {"epsv", "Enter extended passive mode."},
//     {"help", "Returns usage documentation on a command if specified, else a "
//              "general help document is returned."},
//     {"list", "Returns information of a file or directory if specified, else "
//              "information of the current working directory is returned."},
//     {"mkd", "Make directory."},
//     {"mode", "Sets the transfer mode (Stream, Block, or Compressed)."},
//     {"nlst", "Returns a list of file names in a specified directory."},
//     {"noop", "No operation (dummy packet; used mostly on keepalives)."},
//     {"pass", "Authentication password."},
//     {"pasv", "Enter passive mode."},
//     {"port",
//      "Specifies an address and port to which the server should connect."},
//     {"pwd",
//      "Print working directory. Returns the current directory of the host."},
//     {"quit", "Disconnect."},
//     {"rein", "Re initializes the connection."},
//     {"retr", "Retrieve a copy of the file"},
//     {"rmd", "Remove a directory."},
//     {"rnfr", "Rename from."},
//     {"rnto", "Rename to."},
//     {"site", "Sends site specific commands to remote server"},
//     {"smnt", "Mount file structure."},
//     {"stat", "Returns information on the server status, including the status "
//              "of the current connection"},
//     {"stor",
//      "Accept the data and to store the data as a file at the server site"},
//     {"stou", "Store file uniquely."},
//     {"stru", "Set file transfer structure."},
//     {"syst", "Return system type."},
//     {"type", "Sets the transfer mode (ASCII/Binary)."},
//     {"user", "Authentication username."},
//     {"xcup", "Change to the parent of the current working directory"},
//     {"xmkd", "Make a directory"},
// };
