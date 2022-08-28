#ifndef ftr_log_hpp
#define ftr_log_hpp

#include "conf.hpp"
#include <fstream>
#include <memory>
#include <string>

namespace ftr {
class log {
  public:
    log() : m_log_stderr{false} {};
    ~log();

    log(const ftr::log &log) = delete;
    log(ftr::log &&log) = delete;
    ftr::log &operator=(const ftr::log &rhs) = delete;
    ftr::log &operator=(ftr::log &&rhs) = delete;

    void init(const std::string prefix, std::shared_ptr<ftr::conf> conf,
              bool log_stderr);
    void log_err(const std::string msg);
    void log_acc(const std::string msg);

    // research further and fix these
    void log_err(const std::string msg1, const std::string msg2);
    void log_acc(const std::string msg1, const std::string msg2);

  private:
    bool m_log_stderr;
    std::ofstream m_acc_log_stream;
    std::ofstream m_err_log_stream;
    std::string log_msg(const std::string msg);
};

} // namespace ftr
#endif
