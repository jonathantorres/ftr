#ifndef ftr_exception_hpp
#define ftr_exception_hpp

#include <exception>

namespace ftr {

class conf_error : public std::exception {
  public:
    conf_error(const char *what_arg) : what_arg(what_arg) {}
    const char *what() const noexcept { return what_arg; }

  private:
    const char *what_arg;
};

class server_error : public std::exception {
  public:
    server_error(const char *what_arg) : what_arg(what_arg) {}
    const char *what() const noexcept { return what_arg; }

  private:
    const char *what_arg;
};

class session_error : public std::exception {
  public:
    session_error(const char *what_arg) : what_arg(what_arg) {}
    const char *what() const noexcept { return what_arg; }

  private:
    const char *what_arg;
};

} // namespace ftr

#endif
