#ifndef RUNTIME_LOG_SPLITTER_H
#define RUNTIME_LOG_SPLITTER_H

#include <string>
#include <mutex>
#include <filesystem>
#include <iostream>

namespace NanoLogInternal {

class LogSplitter
{
public:
    LogSplitter(const std::string &base_filename, size_t max_size, size_t max_files);
    ~LogSplitter();

    bool checkAndRotate(size_t messageSize);
    FILE* getCurrentFileHandler() { return fHandler_; }

private:
    void rotate_();
    std::string calculate_filename(const std::string &base_filename, size_t index);
    std::pair<std::string, std::string> split_filename(const std::string &filename);
    bool rename_file(const std::string &src_filename, const std::string &target_filename);

    std::string base_filename_;
    size_t max_size_;
    size_t max_files_;
    size_t current_size_;
    std::mutex mutex_;
    FILE *fHandler_;
};

} // namespace NanoLogInternal

#endif