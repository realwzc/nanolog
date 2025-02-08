#include "LogManager.h"
#include <unistd.h>
#include <iostream>

namespace NanoLogInternal {

bool LogManager::initialize(const std::string& logFilePath,
                          const std::string& configPath,
                          std::chrono::milliseconds pollInterval) {
    if (initialized_) {
        std::cerr << "LogManager already initialized" << std::endl;
        return false;
    }

    // Initialize host info
    initializeHostInfo();

    // Initialize log file
    if (!initializeLogFile(logFilePath)) {
        return false;
    }

    // Pre-allocate thread local storage
    NanoLog::preallocate();

    // Create and start config monitor
    try {
        configMonitor_ = std::make_unique<ConfigFileMonitor>(configPath, pollInterval);
        configMonitor_->start();
    } catch (const std::exception& e) {
        std::cerr << "Failed to initialize config monitor: " << e.what() << std::endl;
        return false;
    }

    initialized_ = true;
    std::cout << "Logging system initialized with log level: "
              << static_cast<int>(configMonitor_->getCurrentLogLevel()) << std::endl;

    return true;
}

void LogManager::shutdown() {
    if (!initialized_) {
        return;
    }

    if (configMonitor_) {
        configMonitor_->stop();
        configMonitor_.reset();
    }

    NanoLog::sync();
    initialized_ = false;

    std::cout << "Logging system shutdown complete" << std::endl;
}

NanoLog::LogLevel LogManager::getCurrentLogLevel() const {
    if (configMonitor_) {
        return configMonitor_->getCurrentLogLevel();
    }
    return NanoLog::INFO;
}

void LogManager::printStats() const {
    std::string stats = NanoLog::getStats();
    printf("%s", stats.c_str());
    NanoLog::printConfig();
}

void LogManager::initializeHostInfo() {
    char hostnameBuffer[256];
    size_t len = sizeof(hostnameBuffer);
    
    if (gethostname(hostnameBuffer, len) != 0) {
        std::cerr << "Failed to get hostname, using 'unknown'" << std::endl;
        hostname_ = "unknown";
    } else {
        hostname_ = hostnameBuffer;
    }
    
    pid_ = getpid();
}

bool LogManager::initializeLogFile(const std::string& logFilePath) {
    try {
        NanoLog::setLogFile(logFilePath.c_str(), hostname_.c_str(), pid_);
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to set log file: " << e.what() << std::endl;
        return false;
    }
}

} // namespace NanoLogInternal