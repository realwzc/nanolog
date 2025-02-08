/**
 * LogPerformanceTest.cc
 *
 * 用于测试 NanoLog 日志库的性能，包括日志写入的延时和吞吐量。
 *
 * 编译时请确保包含 NanoLog 的头文件和链接对应的库，同时定义必要的宏（例如-NANOLOG_CPP17 ），
 * 例如（基于 GNUmakefile 的设置）：
 *
 *   g++ -std=c++17 -O2 -DNANOLOG_CPP17 LogPerformanceTest.cc -o LogPerformanceTest -lpthread
 *
 */

#include <chrono>
#include <iostream>
#include <unistd.h>     // for getpid()
#include "NanoLogCpp17.h"
#include "LogManager.h"

// 测试记录延时：记录固定数量的日志消息，并计算平均写入延时
void testLatency(int iterations) {
    // 使用高精度时钟计时（仅测量前台代码执行 NANO_LOG 的时间，不包括实际压缩和磁盘 IO 的 sync 部分）
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        NANO_LOG(INFO, "Latency test: message %d", i);
    }
    auto mid = std::chrono::high_resolution_clock::now();

    // 调用 sync 将缓冲区数据刷新到磁盘
    NanoLog::sync();
    auto end = std::chrono::high_resolution_clock::now();

    auto logDuration = std::chrono::duration_cast<std::chrono::microseconds>(mid - start).count();
    auto syncDuration = std::chrono::duration_cast<std::chrono::microseconds>(end - mid).count();
    double avgLatency = static_cast<double>(logDuration) / iterations;

    std::cout << "===== Latency Test =====" << std::endl;
    std::cout << "Logged " << iterations << " messages in " << logDuration << " us." << std::endl;
    std::cout << "Average logging latency: " << avgLatency << " us per message." << std::endl;
    std::cout << "Flushing logs (sync) took: " << syncDuration << " us." << std::endl;
    std::cout << std::endl;
}

// 测试吞吐量：在设定时间内尽可能多地发送日志消息，并计算平均每秒日志数
void testThroughput(int testDurationSeconds) {
    int messageCount = 0;
    auto start = std::chrono::high_resolution_clock::now();
    auto endTime = start + std::chrono::seconds(testDurationSeconds);
    
    while (std::chrono::high_resolution_clock::now() < endTime) {
        NANO_LOG(INFO, "Throughput test: message %d", messageCount);
        messageCount++;
    }
    
    // 刷新所有日志消息
    NanoLog::sync();
    
    std::cout << "===== Throughput Test =====" << std::endl;
    std::cout << "Logged " << messageCount << " messages in " 
              << testDurationSeconds << " seconds." << std::endl;
    double throughput = static_cast<double>(messageCount) / testDurationSeconds;
    std::cout << "Throughput: " << throughput << " messages per second." << std::endl;
    std::cout << std::endl;
}

int main() {
    // 设置日志文件路径、主机名和进程号
    NanoLog::setLogFile("perf_test.log", "localhost", getpid());
    
    // 初始化 NanoLog 日志系统
    auto &logManager = NanoLogInternal::LogManager::getInstance();
    if (!logManager.initialize()) {
        std::cerr << "Failed to initialize NanoLog logging system." << std::endl;
        return 1;
    }
    
    // 预分配线程局部缓冲区
    NanoLog::preallocate();

    // 延时测试：例如记录 100,000 条日志消息
    const int latencyIterations = 100000;
    testLatency(latencyIterations);

    // 吞吐量测试：运行 5 秒钟内尽可能多的记录日志消息
    int throughputTestDuration = 5; // 單位为秒
    testThroughput(throughputTestDuration);

    // 打印 NanoLog 内部统计信息（可选）
    std::string stats = NanoLog::getStats();
    std::cout << "===== NanoLog Stats =====" << std::endl;
    std::cout << stats << std::endl;

    // 打印当前配置参数（可选）
    NanoLog::printConfig();

    // 清理关闭日志系统
    logManager.shutdown();
    
    return 0;
}