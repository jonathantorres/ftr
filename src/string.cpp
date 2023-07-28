#include "string.hpp"

#include <cctype>
#include <iterator>
#include <list>
#include <string>
#include <string_view>
#include <vector>

using namespace ftr;

std::string ftr::trim(const std::string_view &s) {
    std::string res = trim_left(s);

    return trim_right(res);
}

std::string ftr::trim_whitespace(const std::string_view &s) {
    std::string res = trim_left(s);

    return trim_right(res);
}

std::string ftr::trim_right(const std::string_view &s) {
    auto rev_it = s.rbegin();
    while (rev_it.base() != s.begin() && std::isspace(*rev_it)) {
        rev_it++;
    }

    return std::string(s.begin(), rev_it.base());
}

std::string ftr::trim_left(const std::string_view &s) {
    auto it = s.begin();
    while (it != s.end() && std::isspace(*it)) {
        it++;
    }

    return std::string(it, s.end());
}

std::string ftr::to_lower(const std::string_view &s) {
    std::string res;

    for (auto &c : s) {
        int new_char = c;
        if (std::isalpha(c)) {
            new_char = std::tolower(c);
        }
        res.push_back(new_char);
    }

    return res;
}

std::string ftr::to_upper(const std::string_view &s) {
    std::string res;

    for (auto &c : s) {
        int new_char = c;
        if (std::isalpha(c)) {
            new_char = std::toupper(c);
        }
        res.push_back(new_char);
    }

    return res;
}

std::vector<std::string> ftr::split(const std::string_view &s,
                                    const std::string_view &delim) {
    std::vector<std::string> res;

    if (s.size() == 0) {
        return res;
    }

    if (delim.size() == 0) {
        res.push_back(std::string(s));

        return res;
    }

    std::string_view str(s.data(), s.size());
    std::string word;

    // TODO: this needs to be done in a better way
    for (auto it = str.begin(); it != str.end(); ++it) {
        if (*it != delim.front()) {
            word.push_back(*it);

            if (it == std::next(str.end(), -1)) {
                res.push_back(word);
                word = "";
            }
            continue;
        }

        res.push_back(word);
        word = "";
    }

    return res;
}

template <typename Container>
std::string ftr::join(const Container &contents,
                      const std::string_view &delim) {
    if (contents.size() == 0) {
        return "";
    }

    std::string res;

    for (const std::string &item : contents) {
        res.append(item);
        res.append(delim);
    }

    res.erase(res.size() - delim.size(), delim.size());

    return res;
}

template std::string ftr::join(const std::vector<std::string> &contents,
                               const std::string_view &delim);

template std::string ftr::join(const std::list<std::string> &contents,
                               const std::string_view &delim);

bool ftr::starts_with(const std::string_view &s1, const std::string_view &s2) {
    if (s1.size() == 0 || s2.size() == 0) {
        return false;
    }

    auto n = s1.find(s2);

    if (n == std::string_view::npos) {
        return false;
    }

    if (n == 0) {
        return true;
    }

    return false;
}

bool ftr::ends_with(const std::string_view &s1, const std::string_view &s2) {
    if (s1.size() == 0 || s2.size() == 0) {
        return false;
    }

    auto n = s1.rfind(s2);

    if (n == std::string_view::npos) {
        return false;
    }

    if (n == (s1.size() - s2.size())) {
        return true;
    }

    return false;
}

bool ftr::contains(const std::string_view &s1, const std::string_view &s2) {
    if (s1.size() == 0 || s2.size() == 0) {
        return false;
    }

    auto n = s1.rfind(s2);

    if (n == std::string_view::npos) {
        return false;
    }

    return true;
}
