#ifndef DIRECTORY_MANAGER_HPP
#define DIRECTORY_MANAGER_HPP
#include <filesystem>
#include <vector>

namespace utils::fs {

class directory_manager final {
    std::filesystem::path m_path;

   public:
    directory_manager() = default;
    directory_manager(const std::filesystem::path& p)
        : m_path{p} {};
    directory_manager(const directory_manager&) = default;
    directory_manager(directory_manager&&) = default;
    directory_manager& operator=(const directory_manager&) = default;
    directory_manager& operator=(directory_manager&&) = default;
    ~directory_manager() = default;

    std::vector<std::filesystem::path> resolve_directory() {
        std::vector<std::filesystem::path> files;
        if (std::filesystem::is_directory(m_path)) {
            for (auto it = std::filesystem::recursive_directory_iterator{m_path};
                 it != std::filesystem::recursive_directory_iterator(); it++) {
                auto current_path = *it;
                if (std::filesystem::is_regular_file(current_path)) {
                    files.push_back(current_path);
                };
            }
        } else {
            files.push_back(m_path);
        }
        return files;
    };

    template <typename... Args>
        requires(std::is_convertible_v<Args, std::string_view> && ...)
    static bool validate_extension(const std::vector<std::filesystem::path>& files, Args&&... extensions) {
        for (const auto& f : files) {
            if (((f.extension() != std::forward<Args>(extensions)) && ...)) {
                return false;
            }
        }
        return true;
    };
};
}  // namespace utils::fs

#endif