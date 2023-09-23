#ifndef ftr_exception_hpp
#define ftr_exception_hpp

#include <exception>

namespace ftr {

class ConfError : public std::exception {
  public:
    ConfError(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

class ServerError : public std::exception {
  public:
    ServerError(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

class SessionError : public std::exception {
  public:
    SessionError(const char *what_arg) : m_what(what_arg) {}
    const char *what() const noexcept { return m_what; }

  private:
    const char *m_what;
};

} // namespace ftr

#endif
