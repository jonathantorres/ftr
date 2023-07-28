#ifndef daemon_hpp
#define daemon_hpp

#include <string>

namespace ftr {

void daemonize(const std::string cmd);
bool daemon_is_running(const std::string cmd, const std::string lockfile_path);

} // namespace ftr
#endif
