#include "util.hpp"
#include <cctype>
#include <string>

std::string ftr::trim_whitespace(const std::string &s) {
    auto it = s.begin();
    while (it != s.end() && std::isspace(*it)) {
        it++;
    }

    auto rev_it = s.rbegin();
    while (rev_it.base() != it && std::isspace(*rev_it)) {
        rev_it++;
    }

    return std::string(it, rev_it.base());
}
