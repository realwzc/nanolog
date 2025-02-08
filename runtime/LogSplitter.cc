#include "LogSplitter.h"

namespace NanoLogInternal {

LogSplitter::LogSplitter(const std::string &base_filename, size_t max_size, size_t max_files)
    : base_filename_(base_filename)
    , max_size_(max_size)
    , max_files_(max_files)
    , current_size_(0)
    , fHandler_(nullptr)
{
    std::string filename = calculate_filename(base_filename_, 0);
    fHandler_ = fopen(filename.c_str(), "a");
    if (!fHandler_) {
        throw std::runtime_error("Failed to open log file: " + filename);
    }
    if (std::filesystem::exists(filename)) {
        current_size_ = std::filesystem::file_size(filename);
    }
}

LogSplitter::~LogSplitter()
{
    if (fHandler_)
    {
        fclose(fHandler_);
    }
}

bool LogSplitter::checkAndRotate(size_t messageSize) {
    current_size_ += messageSize;
    if (current_size_ >= max_size_) {
        rotate_();
        return true;  
    }
    return false;   
}
void LogSplitter::rotate_() {
    std::lock_guard<std::mutex> lock(mutex_);
    
    if (fHandler_) {
        fclose(fHandler_);
        fHandler_ = nullptr;
    }

    for (size_t i = max_files_; i > 0; --i) {
        std::string src_filename = calculate_filename(base_filename_, i - 1);
        if (!std::filesystem::exists(src_filename)) {
            continue;
        }
        std::string target_filename = calculate_filename(base_filename_, i);
        if (!rename_file(src_filename, target_filename)) {
            std::cerr << "Failed to rename log file: " << src_filename 
                     << " to " << target_filename << std::endl;
            continue;
        }
    }

    std::string new_filename = calculate_filename(base_filename_, 0);
    fHandler_ = fopen(new_filename.c_str(), "w");  
    
    if (!fHandler_) {
        throw std::runtime_error("Failed to open new log file: " + new_filename);
    }

    current_size_ = 0;

    std::cout << "Rotated to new log file: " << new_filename 
              << " handler: " << fHandler_ << std::endl;
}

bool LogSplitter::rename_file(const std::string &src_filename, const std::string &target_filename)
{
    if (std::filesystem::exists(target_filename))
    {
        std::filesystem::remove(target_filename);
    }
    try
    {
        std::filesystem::rename(src_filename, target_filename);
        return true;
    }
    catch (const std::filesystem::filesystem_error &e)
    {
        std::cerr << "Failed to rename log file: " << e.what() << std::endl;
        return false;
    }
}

std::string LogSplitter::calculate_filename(const std::string &base_filename, size_t index) {
    if (index == 0) {
        return base_filename + ".txt";  
    }
    
    return base_filename + "." + std::to_string(index) + ".txt";
}

std::pair<std::string, std::string> LogSplitter::split_filename(const std::string &filename)
{
    size_t pos = filename.rfind('.');
    if (pos == std::string::npos)
    {
        return {filename, ""};
    }
    return {filename.substr(0, pos), filename.substr(pos + 1)};
}

} // namespace NanoLogInternal