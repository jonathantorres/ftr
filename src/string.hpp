#ifndef string_hpp
#define string_hpp

#include <string>
#include <string_view>
#include <vector>

namespace ftr {

std::string trim(const std::string_view &s);
std::string trim_whitespace(const std::string_view &s);
std::string trim_right(const std::string_view &s);
std::string trim_left(const std::string_view &s);
std::string to_lower(const std::string_view &s);
std::string to_upper(const std::string_view &s);
bool starts_with(const std::string_view &s1, const std::string_view &s2);
bool ends_with(const std::string_view &s1, const std::string_view &s2);
bool contains(const std::string_view &s1, const std::string_view &s2);
std::vector<std::string> split(const std::string_view &s,
                               const std::string_view &delim);

template <typename Container>
std::string join(const Container &contents, const std::string_view &delim);

} // namespace ftr
#endif
