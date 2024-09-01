use std::fmt::{Display, Formatter};

pub use conf::Conf;
pub use log::Log;
pub use server::Server;
pub use session::{Session, Sessions};

pub const VERSION: &str = "0.0.1";
pub const PROG_NAME: &str = "ftr";
pub const DEFAULT_CONF: &str = "ftrd.conf";

// server constants
const DEFAULT_CMD_SIZE: i32 = 512;
const CONTROL_PORT: i32 = 21;
const BACKLOG: i32 = 4096;

const TRANSFER_TYPE_ASCII: &str = "A";
const TRANSFER_TYPE_IMG: &str = "I";
const DEFAULT_NAME: &str = "localhost";

// file related constants
const DIR_SIZE: i32 = 4096;
const FILE_BUF: i32 = 4096;

// status codes
#[derive(Debug, Copy, Clone)]
pub enum StatusCode {
    RestartMarker = 110,
    ReadyInAFew = 120,
    DataConnAlreadyOpen = 125,
    FileStatusOk = 150,
    OK = 200,
    NotImplemented = 202,
    SystemStatus = 211,
    DirStatus = 212,
    FileStatus = 213,
    HelpMessage = 214,
    NameSystem = 215,
    ServiceReady = 220,
    ClosingControlConn = 221,
    DataConnOpen = 225,
    ClosingDataConn = 226,
    EnterPassMode = 227,
    EnterLongPassMode = 228,
    EnterExtPassMode = 229,
    UserLoggedIn = 230,
    UserLoggedOut = 231,
    LogoutCmdNoted = 232,
    AuthAccepted = 234,
    RequestedFileOk = 250,
    PathCreated = 257,
    UsernameOk = 331,
    NeedAccount = 332,
    RequestedFileAction = 350,
    CmdNotAccepted = 400,
    ServiceNotAvailable = 421,
    CantOpenDataConn = 425,
    ConnClosed = 426,
    InvalidUsername = 430,
    HostUnavailable = 434,
    FileActionNotTaken = 450,
    ActionAborted = 451,
    ActionNotTaken = 452,
    UnknownError = 500,
    SyntaxError = 501,
    CmdNotImplemented = 502,
    BadSequence = 503,
    CmdNotImplementedForParam = 504,
    ExtPortUnknownProtocol = 522,
    NotLoggedIn = 530,
    NeedAccountForStoring = 532,
    CouldNotConnToServer = 534,
    FileNotFound = 550,
    RequestedActionAborted = 551,
    RequestedFileActionAborted = 552,
    RequestedActionNotTaken = 553,
    IntegrityProtectedReply = 631,
    ConfAndIntegrityProtectedReply = 632,
    ConfProtectedReply = 633,
}

impl StatusCode {
    pub fn code(self) -> i32 {
        self as i32
    }
}

impl Display for StatusCode {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result<(), std::fmt::Error> {
        match self {
            StatusCode::RestartMarker => write!(f, "Restart marker replay."),
            StatusCode::ReadyInAFew => write!(f, "Service ready in a few minutes."),
            StatusCode::DataConnAlreadyOpen => write!(f, "Data connection already open."),
            StatusCode::FileStatusOk => {
                write!(f, "File status okay, about to open data connection.")
            }
            StatusCode::OK => write!(f, "Command Ok."),
            StatusCode::NotImplemented => write!(f, "Command not implemented."),
            StatusCode::SystemStatus => write!(f, "System Status."),
            StatusCode::DirStatus => write!(f, "Directory Status."),
            StatusCode::FileStatus => write!(f, "File Status."),
            StatusCode::HelpMessage => write!(f, "Help Message."),
            StatusCode::NameSystem => write!(f, "NAME system type."),
            StatusCode::ServiceReady => write!(f, "Service Ready."),
            StatusCode::ClosingControlConn => write!(f, "Closing control connection."),
            StatusCode::DataConnOpen => write!(f, "Data connection open, no transfer in progress."),
            StatusCode::ClosingDataConn => write!(f, "Closing data connection. File action ok."),
            StatusCode::EnterPassMode => write!(f, "Entering passive mode."),
            StatusCode::EnterLongPassMode => write!(f, "Entering long passive mode."),
            StatusCode::EnterExtPassMode => write!(f, "Entering extended passive mode."),
            StatusCode::UserLoggedIn => {
                write!(f, "User logged in, proceed. Logged out if appropriate.")
            }
            StatusCode::UserLoggedOut => write!(f, "User logged out, service terminated."),
            StatusCode::LogoutCmdNoted => write!(f, "Logout command noted."),
            StatusCode::AuthAccepted => write!(f, "Authentication mechanism accepted."),
            StatusCode::RequestedFileOk => write!(f, "Requested file action ok, completed."),
            StatusCode::PathCreated => write!(f, "Path created."),
            StatusCode::UsernameOk => write!(f, "Username okay, need password."),
            StatusCode::NeedAccount => write!(f, "Need account for login."),
            StatusCode::RequestedFileAction => {
                write!(f, "Requested file action pending more information.")
            }
            StatusCode::CmdNotAccepted => write!(f, "Command not accepted, please try again."),
            StatusCode::ServiceNotAvailable => {
                write!(f, "Service not available, closing control connection.")
            }
            StatusCode::CantOpenDataConn => write!(f, "Can't open data connection."),
            StatusCode::ConnClosed => write!(f, "Connection closed, transfer aborted."),
            StatusCode::InvalidUsername => write!(f, "Invalid username or password."),
            StatusCode::HostUnavailable => write!(f, "Requested host unavailable."),
            StatusCode::FileActionNotTaken => write!(f, "Requested file action not taken."),
            StatusCode::ActionAborted => {
                write!(f, "Requested action aborted. Local error in processing.")
            }
            StatusCode::ActionNotTaken => write!(f, "Requested action not taken. Insufficient "),
            StatusCode::UnknownError => write!(f, "Unknown error."),
            StatusCode::SyntaxError => write!(f, "Syntax error in parameters or arguments."),
            StatusCode::CmdNotImplemented => write!(f, "Command not implemented."),
            StatusCode::BadSequence => write!(f, "Bad sequence of commands."),
            StatusCode::CmdNotImplementedForParam => {
                write!(f, "Command not implemented for that parameter.")
            }
            StatusCode::ExtPortUnknownProtocol => {
                write!(f, "Extended Port Failure - unknown network protocol.")
            }
            StatusCode::NotLoggedIn => write!(f, "Not logged in."),
            StatusCode::NeedAccountForStoring => write!(f, "Need account for storing files."),
            StatusCode::CouldNotConnToServer => {
                write!(f, "Could Not Connect to Server - Policy Requires SSL.")
            }
            StatusCode::FileNotFound => write!(f, "File not found, error encountered."),
            StatusCode::RequestedActionAborted => {
                write!(f, "Requested action aborted. Page type unknown.")
            }
            StatusCode::RequestedFileActionAborted => write!(
                f,
                "Requested file action aborted. Exceeded storage allocation."
            ),
            StatusCode::RequestedActionNotTaken => {
                write!(f, "Requested action not taken. File name not allowed.")
            }
            StatusCode::IntegrityProtectedReply => write!(f, "Integrity protected reply."),
            StatusCode::ConfAndIntegrityProtectedReply => {
                write!(f, "Confidentiality and integrity protected reply.")
            }
            StatusCode::ConfProtectedReply => write!(f, "Confidentiality protected reply."),
        }
    }
}

// commands
#[derive(Debug, Copy, Clone)]
enum Cmd {
    Abort,
    Account,
    AuthData,
    Allo,
    Append,
    Auth,
    Avail,
    Clear,
    ChangeParent,
    Conf,
    CsId,
    ChangeDir,
    Delete,
    DirSize,
    PrivProtected,
    ExtAddrPort,
    ExtPassvMode,
    FeatList,
    Help,
    Host,
    Lang,
    List,
    LongAddrPort,
    LongPassvMode,
    LongModTime,
    ModCreateTime,
    ModFact,
    ModLastModTime,
    InteProtect,
    MakeDir,
    ListDir,
    ObjData,
    Mode,
    FileNames,
    Noop,
    Options,
    Password,
    Passive,
    BufSizePort,
    Port,
    DataChanProtoLvl,
    PrintDir,
    Quit,
    Reinit,
    Restart,
    Retrieve,
    RemoveDir,
    RemoveDirTree,
    RenameFrom,
    RenameTo,
    Site,
    FileSize,
    MountFile,
    SinglePortPassv,
    ServerStatus,
    AcceptAndStore,
    StoreFile,
    FileStruct,
    SystemType,
    Thumbnail,
    Type,
    User,
    ChangeToParentDir,
    MakeADir,
    PrintCurDir,
    RemoveTheDir,
    SendMail,
    SendTerm,
}

impl Display for Cmd {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result<(), std::fmt::Error> {
        match self {
            Cmd::Abort => write!(f, "ABOR"),
            Cmd::Account => write!(f, "ACCT"),
            Cmd::AuthData => write!(f, "ADAT"),
            Cmd::Allo => write!(f, "ALLO"),
            Cmd::Append => write!(f, "APPE"),
            Cmd::Auth => write!(f, "AUTH"),
            Cmd::Avail => write!(f, "AVBL"),
            Cmd::Clear => write!(f, "CCC"),
            Cmd::ChangeParent => write!(f, "CDUP"),
            Cmd::Conf => write!(f, "CONF"),
            Cmd::CsId => write!(f, "CSID"),
            Cmd::ChangeDir => write!(f, "CWD"),
            Cmd::Delete => write!(f, "DELE"),
            Cmd::DirSize => write!(f, "DSIZ"),
            Cmd::PrivProtected => write!(f, "ENC"),
            Cmd::ExtAddrPort => write!(f, "EPRT"),
            Cmd::ExtPassvMode => write!(f, "EPSV"),
            Cmd::FeatList => write!(f, "FEAT"),
            Cmd::Help => write!(f, "HELP"),
            Cmd::Host => write!(f, "HOST"),
            Cmd::Lang => write!(f, "LANG"),
            Cmd::List => write!(f, "LIST"),
            Cmd::LongAddrPort => write!(f, "LPRT"),
            Cmd::LongPassvMode => write!(f, "LPSV"),
            Cmd::LongModTime => write!(f, "MDTM"),
            Cmd::ModCreateTime => write!(f, "MFCT"),
            Cmd::ModFact => write!(f, "MFF"),
            Cmd::ModLastModTime => write!(f, "MFMT"),
            Cmd::InteProtect => write!(f, "MIC"),
            Cmd::MakeDir => write!(f, "MKD"),
            Cmd::ListDir => write!(f, "MLSD"),
            Cmd::ObjData => write!(f, "MLST"),
            Cmd::Mode => write!(f, "MODE"),
            Cmd::FileNames => write!(f, "NLST"),
            Cmd::Noop => write!(f, "NOOP"),
            Cmd::Options => write!(f, "OPTS"),
            Cmd::Password => write!(f, "PASS"),
            Cmd::Passive => write!(f, "PASV"),
            Cmd::BufSizePort => write!(f, "PBSZ"),
            Cmd::Port => write!(f, "PORT"),
            Cmd::DataChanProtoLvl => write!(f, "PROT"),
            Cmd::PrintDir => write!(f, "PWD"),
            Cmd::Quit => write!(f, "QUIT"),
            Cmd::Reinit => write!(f, "REIN"),
            Cmd::Restart => write!(f, "REST"),
            Cmd::Retrieve => write!(f, "RETR"),
            Cmd::RemoveDir => write!(f, "RMD"),
            Cmd::RemoveDirTree => write!(f, "RMDA"),
            Cmd::RenameFrom => write!(f, "RNFR"),
            Cmd::RenameTo => write!(f, "RNTO"),
            Cmd::Site => write!(f, "SITE"),
            Cmd::FileSize => write!(f, "SIZE"),
            Cmd::MountFile => write!(f, "SMNT"),
            Cmd::SinglePortPassv => write!(f, "SPSV"),
            Cmd::ServerStatus => write!(f, "STAT"),
            Cmd::AcceptAndStore => write!(f, "STOR"),
            Cmd::StoreFile => write!(f, "STOU"),
            Cmd::FileStruct => write!(f, "STRU"),
            Cmd::SystemType => write!(f, "SYST"),
            Cmd::Thumbnail => write!(f, "THMB"),
            Cmd::Type => write!(f, "TYPE"),
            Cmd::User => write!(f, "USER"),
            Cmd::ChangeToParentDir => write!(f, "XCUP"),
            Cmd::MakeADir => write!(f, "XMKD"),
            Cmd::PrintCurDir => write!(f, "XPWD"),
            Cmd::RemoveTheDir => write!(f, "XRMD"),
            Cmd::SendMail => write!(f, "XSEM"),
            Cmd::SendTerm => write!(f, "XSEN"),
        }
    }
}

// TODO: implement the From trait

// const CMD_ABORT: &str = "ABOR";
// const CMD_ACCOUNT: &str = "ACCT";
// const CMD_AUTH_DATA: &str = "ADAT";
// const CMD_ALLO: &str = "ALLO";
// const CMD_APPEND: &str = "APPE";
// const CMD_AUTH: &str = "AUTH";
// const CMD_AVAIL: &str = "AVBL";
// const CMD_CLEAR: &str = "CCC";
// const CMD_CHANGE_PARENT: &str = "CDUP";
// const CMD_CONF: &str = "CONF";
// const CMD_CS_ID: &str = "CSID";
// const CMD_CHANGE_DIR: &str = "CWD";
// const CMD_DELETE: &str = "DELE";
// const CMD_DIR_SIZE: &str = "DSIZ";
// const CMD_PRIV_PROTECTED: &str = "ENC";
// const CMD_EXT_ADDR_PORT: &str = "EPRT";
// const CMD_EXT_PASSV_MODE: &str = "EPSV";
// const CMD_FEAT_LIST: &str = "FEAT";
// const CMD_HELP: &str = "HELP";
// const CMD_HOST: &str = "HOST";
// const CMD_LANG: &str = "LANG";
// const CMD_LIST: &str = "LIST";
// const CMD_LONG_ADDR_PORT: &str = "LPRT";
// const CMD_LONG_PASSV_MODE: &str = "LPSV";
// const CMD_LONG_MOD_TIME: &str = "MDTM";
// const CMD_MOD_CREATE_TIME: &str = "MFCT";
// const CMD_MOD_FACT: &str = "MFF";
// const CMD_MOD_LAST_MOD_TIME: &str = "MFMT";
// const CMD_INTE_PROTECT: &str = "MIC";
// const CMD_MAKE_DIR: &str = "MKD";
// const CMD_LIST_DIR: &str = "MLSD";
// const CMD_OBJ_DATA: &str = "MLST";
// const CMD_MODE: &str = "MODE";
// const CMD_FILE_NAMES: &str = "NLST";
// const CMD_NOOP: &str = "NOOP";
// const CMD_OPTIONS: &str = "OPTS";
// const CMD_PASSWORD: &str = "PASS";
// const CMD_PASSIVE: &str = "PASV";
// const CMD_BUF_SIZE_PORT: &str = "PBSZ";
// const CMD_PORT: &str = "PORT";
// const CMD_DATA_CHAN_PROTO_LVL: &str = "PROT";
// const CMD_PRINT_DIR: &str = "PWD";
// const CMD_QUIT: &str = "QUIT";
// const CMD_REINIT: &str = "REIN";
// const CMD_RESTART: &str = "REST";
// const CMD_RETRIEVE: &str = "RETR";
// const CMD_REMOVE_DIR: &str = "RMD";
// const CMD_REMOVE_DIR_TREE: &str = "RMDA";
// const CMD_RENAME_FROM: &str = "RNFR";
// const CMD_RENAME_TO: &str = "RNTO";
// const CMD_SITE: &str = "SITE";
// const CMD_FILE_SIZE: &str = "SIZE";
// const CMD_MOUNT_FILE: &str = "SMNT";
// const CMD_SINGLE_PORT_PASSIV: &str = "SPSV";
// const CMD_SERVER_STATUS: &str = "STAT";
// const CMD_ACCEPT_AND_STORE: &str = "STOR";
// const CMD_STORE_FILE: &str = "STOU";
// const CMD_FILE_STRUCT: &str = "STRU";
// const CMD_SYSTEM_TYPE: &str = "SYST";
// const CMD_THUMBNAIL: &str = "THMB";
// const CMD_TYPE: &str = "TYPE";
// const CMD_USER: &str = "USER";
// const CMD_CHANGE_TO_PARENT_DIR: &str = "XCUP";
// const CMD_MAKE_A_DIR: &str = "XMKD";
// const CMD_PRINT_CUR_DIR: &str = "XPWD";
// const CMD_REMOVE_THE_DIR: &str = "XRMD";
// const CMD_SEND_MAIL: &str = "XSEM";
// const CMD_SEND_TERM: &str = "XSEN";

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
