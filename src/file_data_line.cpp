#include "file_data_line.hpp"
#include "constants.hpp"
#include <chrono>
#include <filesystem>
#include <grp.h>
#include <iomanip>
#include <pwd.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

using namespace ftr;

file_data_line::file_data_line(const std::filesystem::directory_entry &entry)
    : m_dir_entry{entry}, m_owner_user_id{0}, m_owner_group_id{0} {
    struct stat file_stat = {};

    if (stat(m_dir_entry.path().c_str(), &file_stat) < 0) {
        return;
    }

    m_owner_user_id = file_stat.st_uid;
    m_owner_group_id = file_stat.st_gid;
}

std::string file_data_line::get_file_line() {
    std::stringstream file_data;
    std::filesystem::file_status entry_status = m_dir_entry.status();
    std::filesystem::perms entry_perms = entry_status.permissions();

    file_data << get_permissions_line(m_dir_entry.is_directory(), entry_perms);
    file_data << ' ';
    file_data << get_owner_name();
    file_data << ' ';
    file_data << get_group_name();
    file_data << ' ';

    if (m_dir_entry.is_directory()) {
        file_data << ftr::DIR_SIZE << ' ';
    } else {
        file_data << m_dir_entry.file_size() << ' ';
    }

    std::chrono::sys_time system_time =
        std::chrono::file_clock::to_sys(m_dir_entry.last_write_time());
    std::time_t cur_time = std::chrono::system_clock::to_time_t(system_time);
    file_data << std::put_time(std::localtime(&cur_time), "%h %d %H:%M");
    file_data << ' ';
    file_data << m_dir_entry.path().filename().string();

    return file_data.str();
}

std::string file_data_line::get_owner_name() {
    std::string own_name;

    if (m_owner_user_id == 0) {
        return own_name;
    }

    struct passwd *pass_data = getpwuid(m_owner_user_id);

    if (pass_data == nullptr) {
        // TODO: Log this error
        return own_name;
    }

    own_name = pass_data->pw_name;

    return own_name;
}

std::string file_data_line::get_group_name() {
    std::string grp_name;

    if (m_owner_group_id == 0) {
        return grp_name;
    }

    struct group *grp_data = getgrgid(m_owner_group_id);

    if (grp_data == nullptr) {
        // TODO: log this error
        return grp_name;
    }

    grp_name = grp_data->gr_name;

    return grp_name;
}

std::string file_data_line::get_permissions_line(
    bool is_directory, const std::filesystem::perms &entry_perms) {
    std::stringstream perms_data;

    if (is_directory) {
        perms_data << 'd';
    } else {
        perms_data << '-';
    }

    perms_data << ((entry_perms & std::filesystem::perms::owner_read) !=
                           std::filesystem::perms::none
                       ? 'r'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::owner_write) !=
                           std::filesystem::perms::none
                       ? 'w'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::owner_exec) !=
                           std::filesystem::perms::none
                       ? 'x'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::group_read) !=
                           std::filesystem::perms::none
                       ? 'r'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::group_write) !=
                           std::filesystem::perms::none
                       ? 'w'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::group_exec) !=
                           std::filesystem::perms::none
                       ? 'x'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::others_read) !=
                           std::filesystem::perms::none
                       ? 'r'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::others_write) !=
                           std::filesystem::perms::none
                       ? 'w'
                       : '-');

    perms_data << ((entry_perms & std::filesystem::perms::others_exec) !=
                           std::filesystem::perms::none
                       ? 'x'
                       : '-');

    return perms_data.str();
}
