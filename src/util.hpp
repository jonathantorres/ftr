#ifndef ftr_util_hpp
#define ftr_util_hpp

#include <string>
#include <vector>

namespace ftr {

std::string trim_whitespace(const std::string &s);
std::string trim_right(const std::string &s);
std::string trim_left(const std::string &s);
std::string to_lower(const std::string &s);
std::string to_upper(const std::string &s);
std::vector<std::string> split(const std::string &s, const std::string &delim);
bool is_ipv4(const std::string &s);
bool is_ipv6(const std::string &s);
bool is_domain_name(const std::string &s);

} // namespace ftr
#endif
