#include "util.hpp"
#include <cctype>
#include <regex>
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

std::string ftr::trim_right(const std::string &s) {
    auto rev_it = s.rbegin();
    while (rev_it.base() != s.begin() && std::isspace(*rev_it)) {
        rev_it++;
    }

    return std::string(s.begin(), rev_it.base());
}

std::string ftr::trim_left(const std::string &s) {
    auto it = s.begin();
    while (it != s.end() && std::isspace(*it)) {
        it++;
    }

    return std::string(it, s.end());
}

bool ftr::is_ipv4(const std::string &s) {
    const std::regex ipv4_regex(
        R"regex((\b25[0-5]|\b2[0-4][0-9]|\b[01]?[0-9][0-9]?)(\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)){3})regex");

    return std::regex_match(s, ipv4_regex);
}

bool ftr::is_ipv6(const std::string &s) {
    const std::regex ipv6_regex(
        R"regex((([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])))regex");

    return std::regex_match(s, ipv6_regex);
}

bool ftr::is_domain_name(const std::string &s) {
    const std::regex domain_regex(
        R"regex((?:[a-z0-9](?:[a-z0-9-]{0,61}[a-z0-9])?\.)+[a-z0-9][a-z0-9-]{0,61}[a-z0-9])regex");

    return std::regex_match(s, domain_regex);
}
