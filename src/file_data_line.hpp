#ifndef ftr_file_data_line_hpp
#define ftr_file_data_line_hpp

#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace ftr {
class file_data_line {
  public:
    file_data_line(const std::filesystem::directory_entry &entry);
    ~file_data_line() = default;

    file_data_line(const file_data_line &other) = delete;
    file_data_line(file_data_line &&other) = delete;
    file_data_line &operator=(const file_data_line &rhs) = delete;
    file_data_line &operator=(file_data_line &&rhs) = delete;

    std::string get_file_line();

  private:
    const std::filesystem::directory_entry &m_dir_entry;
    uid_t m_owner_user_id;
    gid_t m_owner_group_id;
    std::string get_owner_name();
    std::string get_group_name();
    std::string get_permissions_line(bool is_directory,
                                     const std::filesystem::perms &entry_perms);
};

} // namespace ftr
#endif
