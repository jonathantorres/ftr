#ifndef ftr_util_hpp
#define ftr_util_hpp

#include <string>

namespace ftr {

bool is_ipv4(const std::string &s);
bool is_ipv6(const std::string &s);
bool is_domain_name(const std::string &s);

} // namespace ftr
#endif
