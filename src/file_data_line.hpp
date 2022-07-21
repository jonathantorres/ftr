#ifndef ftr_file_data_line_hpp
#define ftr_file_data_line_hpp

#include <filesystem>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

namespace ftr {
class FileDataLine {
  public:
    FileDataLine(const std::filesystem::directory_entry &entry);
    ~FileDataLine() = default;

    FileDataLine(const FileDataLine &other) = delete;
    FileDataLine(FileDataLine &&other) = delete;
    FileDataLine &operator=(const FileDataLine &rhs) = delete;
    FileDataLine &operator=(FileDataLine &&rhs) = delete;

    std::string get_file_line();

  private:
    const std::filesystem::directory_entry &dir_entry;
    uid_t owner_user_id;
    gid_t owner_group_id;
    std::string get_owner_name();
    std::string get_group_name();
    std::string get_permissions_line(bool is_directory,
                                     const std::filesystem::perms &entry_perms);
};

} // namespace ftr
#endif
