#ifndef RUNTIME_CONFIG_MONITOR_H
#define RUNTIME_CONFIG_MONITOR_H

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <mutex>
#include "NanoLog.h"

namespace NanoLogInternal {

/**
 * ConfigFileMonitor monitors the NanoLog configuration file for changes 
 * and updates logging settings dynamically.
 */
class ConfigFileMonitor {
public:
    /**
     * Constructor that initializes the config file monitor
     * 
     * @param configPath Path to the configuration file to monitor
     * @param pollInterval How often to check for config changes in milliseconds
     */
    explicit ConfigFileMonitor(const std::string& configPath, 
                             std::chrono::milliseconds pollInterval = std::chrono::milliseconds(1000));
    
    ~ConfigFileMonitor();

    /**
     * Starts the monitoring thread
     */
    void start();

    /**
     * Stops the monitoring thread
     */
    void stop();

    /**
     * Gets the current log level from config
     */
    NanoLog::LogLevel getCurrentLogLevel() const;

private:
    /**
     * Main monitoring loop that runs in a separate thread
     */
    void monitorLoop();

    /**
     * Reads and parses the config file
     * @return true if config was successfully parsed
     */
    bool parseConfig();

    /**
     * Updates NanoLog settings based on new config values
     */
    void applyConfig();

    /**
     * Gets the last modification time of the config file
     */
    std::filesystem::file_time_type getConfigFileModTime() const;

    const std::string configPath_;
    const std::chrono::milliseconds pollInterval_;
    
    std::atomic<bool> running_{false};
    std::thread monitorThread_;
    
    mutable std::mutex configMutex_;
    NanoLog::LogLevel currentLogLevel_{NanoLog::LogLevel::INFO};
    std::filesystem::file_time_type lastModTime_;
};

} // namespace NanoLogInternal

#endif // RUNTIME_CONFIG_MONITOR_H
