#ifndef ftr_exception_hpp
#define ftr_exception_hpp

#include <exception>

namespace ftr {

class ConfError : public std::exception {
  public:
    ConfError(const char *what_arg) : what_arg(what_arg) {}
    const char *what() const noexcept { return what_arg; }

  private:
    const char *what_arg;
};

} // namespace ftr

#endif
