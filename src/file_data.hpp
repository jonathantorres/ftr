#ifndef ftr_file_data
#define ftr_file_data

#include "log.hpp"
#include <filesystem>
#include <memory>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace ftr {
class FileData {
  public:
    FileData(const std::filesystem::directory_entry &entry,
             std::shared_ptr<ftr::Log> log);
    ~FileData() = default;

    FileData(const FileData &other) = delete;
    FileData(FileData &&other) = delete;
    FileData &operator=(const FileData &rhs) = delete;
    FileData &operator=(FileData &&rhs) = delete;

    std::string get_file_line();

  private:
    const std::filesystem::directory_entry &m_dir_entry;
    uid_t m_owner_user_id;
    gid_t m_owner_group_id;
    std::shared_ptr<ftr::Log> m_log;
    std::string get_owner_name();
    std::string get_group_name();
    std::string get_permissions_line(bool is_directory,
                                     const std::filesystem::perms &entry_perms);
};

} // namespace ftr
#endif
