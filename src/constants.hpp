#ifndef ftr_status_codes_hpp
#define ftr_status_codes_hpp

#include <map>
#include <string>

namespace ftr {
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
const int STATUS_CODE_SYTAX_ERROR = 501;
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

// TODO: finish this
const std::map<int, std::string> status_codes = {
    {STATUS_CODE_RESTART_MARKER, "Restart marker replay."},
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

} // namespace ftr

#endif
