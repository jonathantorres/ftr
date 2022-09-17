#ifndef ftr_exception_hpp
#define ftr_exception_hpp

#include <exception>

namespace ftr {

class conf_error : public std::exception {
  public:
    conf_error(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

class server_error : public std::exception {
  public:
    server_error(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

class session_error : public std::exception {
  public:
    session_error(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

} // namespace ftr

#endif
