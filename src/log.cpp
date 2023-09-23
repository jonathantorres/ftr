#include "log.hpp"
#include "conf.hpp"
#include "exception.hpp"
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>

using namespace ftr;

void Log::init(const std::string prefix, std::shared_ptr<ftr::Conf> conf,
               bool log_stderr) {
    if (conf == nullptr) {
        throw ServerError("A conf object must be provided");
    }

    const std::string err_log_path = conf->get_error_log();

    if (err_log_path == "") {
        throw ServerError(
            "A path to the location of the error log must be provided");
    }

    const std::string acc_log_path = conf->get_access_log();

    if (acc_log_path == "") {
        throw ServerError(
            "A path to the location of the access log must be provided");
    }

    std::ios_base::openmode mode = std::ios_base::out;
    mode |= std::ios_base::app;

    // try opening the files
    m_err_log_stream.open(prefix + err_log_path, mode);

    if (!m_err_log_stream.is_open()) {
        throw ServerError(std::strerror(errno));
    }

    m_acc_log_stream.open(prefix + acc_log_path, mode);

    if (!m_acc_log_stream.is_open()) {
        throw ServerError(std::strerror(errno));
    }

    if (log_stderr) {
        m_log_stderr = true;
    }
}

Log::~Log() {
    if (m_acc_log_stream.is_open()) {
        m_acc_log_stream.close();
    }

    if (m_err_log_stream.is_open()) {
        m_err_log_stream.close();
    }
}

void Log::log_err(const std::string msg) {
    std::string cur_msg = log_msg(msg);

    m_err_log_stream.write(cur_msg.c_str(), cur_msg.size());
    m_err_log_stream.flush();

    if (m_log_stderr) {
        std::cerr << cur_msg;
    }
}

void Log::log_err(const std::string msg1, const std::string msg2) {
    log_err(msg1 + msg2);
}

void Log::log_acc(const std::string msg) {
    std::string cur_msg = log_msg(msg);

    m_acc_log_stream.write(cur_msg.c_str(), cur_msg.size());
    m_acc_log_stream.flush();

    if (m_log_stderr) {
        std::cerr << cur_msg;
    }
}

void Log::log_acc(const std::string msg1, const std::string msg2) {
    log_acc(msg1 + msg2);
}

std::string Log::log_msg(const std::string msg) {
    std::time_t cur_time =
        std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::ostringstream ss;
    ss << '[';
    ss << std::put_time(std::localtime(&cur_time), "%Y/%m/%d %H:%M:%S");
    ss << "] ";
    ss << msg;
    ss << '\n';

    return ss.str();
}
