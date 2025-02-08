#include "ConfigFileMonitor.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <iostream>

namespace NanoLogInternal {

ConfigFileMonitor::ConfigFileMonitor(const std::string& configPath,
                                   std::chrono::milliseconds pollInterval)
    : configPath_(configPath)
    , pollInterval_(pollInterval)
    , lastModTime_(getConfigFileModTime())
{
    // Initial config parse
    parseConfig();
}

ConfigFileMonitor::~ConfigFileMonitor() {
    stop();
}

void ConfigFileMonitor::start() {
    bool expected = false;
    if (running_.compare_exchange_strong(expected, true)) {
        monitorThread_ = std::thread(&ConfigFileMonitor::monitorLoop, this);
    }
}

void ConfigFileMonitor::stop() {
    running_ = false;
    if (monitorThread_.joinable()) {
        monitorThread_.join();
    }
}

NanoLog::LogLevel ConfigFileMonitor::getCurrentLogLevel() const {
    std::lock_guard<std::mutex> lock(configMutex_);
    return currentLogLevel_;
}

void ConfigFileMonitor::monitorLoop() {
    while (running_) {
        auto currentModTime = getConfigFileModTime();
        if (currentModTime > lastModTime_) {
            if (parseConfig()) {
                applyConfig();
                lastModTime_ = currentModTime;
            }
        }
        std::this_thread::sleep_for(pollInterval_);
    }
}

bool ConfigFileMonitor::parseConfig() {
    std::ifstream configFile(configPath_);
    if (!configFile) {
        std::cerr << "Failed to open config file: " << configPath_ << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(configFile, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string key, equals, value;
        
        if (!(iss >> key >> equals >> value) || equals != "=") {
            continue;
        }

        if (key == "log_level") {
            std::lock_guard<std::mutex> lock(configMutex_);
            if (value == "ERROR") {
                currentLogLevel_ = NanoLog::ERROR;
            } else if (value == "WARNING") {
                currentLogLevel_ = NanoLog::WARN;  
            } else if (value == "INFO") {
                currentLogLevel_ = NanoLog::INFO;
            } else if (value == "DEBUG") {
                currentLogLevel_ = NanoLog::DEBUG;
            }
        }
    }

    return true;
}

void ConfigFileMonitor::applyConfig() {
    std::lock_guard<std::mutex> lock(configMutex_);
    NanoLog::setLogLevel(currentLogLevel_);
}

std::filesystem::file_time_type ConfigFileMonitor::getConfigFileModTime() const {
    try {
        return std::filesystem::last_write_time(configPath_);
    } catch (const std::filesystem::filesystem_error& e) {
        std::cerr << "Error getting config file modification time: " << e.what() << std::endl;
        return std::filesystem::file_time_type::min();  // 返回最小时间点
    }
}

} // namespace NanoLogInternal
