#ifndef RUNTIME_LOG_MANAGER_H
#define RUNTIME_LOG_MANAGER_H

#include <memory>
#include <string>
#include "NanoLog.h"
#include "ConfigFileMonitor.h"
#include "LogSplitter.h"

namespace NanoLogInternal {

class LogManager {
public:
    /**
     * Get singleton instance
     */
    static LogManager& getInstance() {
        static LogManager instance;
        return instance;
    }

    /**
     * Initialize the logging system
     * 
     * @param logFilePath Path for log file
     * @param configPath Path for config file
     * @param pollInterval Config check interval
     * @return true if initialization successful
     */
    bool initialize(const std::string& logFilePath = "app.log",
                   const std::string& configPath = "nanolog.conf",
                   std::chrono::milliseconds pollInterval = std::chrono::milliseconds(1000));

    /**
     * Cleanup logging system resources
     */
    void shutdown();

    /**
     * Get current log level
     */
    NanoLog::LogLevel getCurrentLogLevel() const;

    /**
     * Print statistics about logging
     */
    void printStats() const;

private:
    LogManager() = default;
    ~LogManager() = default;
    
    LogManager(const LogManager&) = delete;
    LogManager& operator=(const LogManager&) = delete;

    bool initializeLogFile(const std::string& logFilePath);
    void initializeHostInfo();

    std::unique_ptr<ConfigFileMonitor> configMonitor_;
    std::string hostname_;
    pid_t pid_{0};
    bool initialized_{false};
};

} // namespace NanoLogInternal

#endif // RUNTIME_LOG_MANAGER_H