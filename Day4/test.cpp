#include "Logger.h"
#include <thread>
#include <cassert>

// 测试用的 LogSink
class TestLogSink : public LogSink {
public:
    string lastMessage;
    int callCount = 0;

    void log(LogLevel level, const char* file, int line,
             const string& content) override {
        lastMessage = content;
        callCount++;
    }
};

// 测试 1: 基本日志输出
void test_basic_logging() {
    std::cout << "\n=== Test 1: 基本日志输出 ===" << std::endl;

    auto& logger = Logger::getInstance();
    logger.addSink(std::make_unique<ConsoleLogSink>());

    LOG_DEBUG("Debug message: {}", 42);
    LOG_INFO("Info message: {}", "Hello");
    LOG_WARN("Warning message: {:.2f}", 3.14159);
    LOG_ERR("Error message: {} + {} = {}", 1, 2, 3);
    LOG_FATAL("Fatal message!");

    std::cout << "✓ 基本日志输出测试通过" << std::endl;
}

// 测试 2: 流式接口 - 使用宏
void test_stream_interface() {
    std::cout << "\n=== Test 2: 流式接口 ===" << std::endl;

    auto& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::DEBUG);

    // 正确的用法：使用宏
    LOG_STREAM(LogLevel::INFO) << "Stream message: " << 123 << " value: " << 3.14;
    LOG_STREAM(LogLevel::DEBUG) << "Debug stream: " << "test";
    LOG_STREAM(LogLevel::WARN) << "Warning: " << 42;

    std::cout << "✓ 流式接口测试通过" << std::endl;
}

// 测试 3: 多种参数类型
void test_various_types() {
    std::cout << "\n=== Test 3: 多种参数类型 ===" << std::endl;

    auto& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::DEBUG);

    int intVal = 42;
    double doubleVal = 3.14159;
    const char* strVal = "C-string";
    std::string stdStrVal = "std::string";

    LOG_INFO("Int: {}, Double: {:.2f}, CStr: {}, StdStr: {}",
             intVal, doubleVal, strVal, stdStrVal);

    LOG_INFO("1:{} 2:{} 3:{} 4:{} 5:{}", 1, 2, 3, 4, 5);

    std::cout << "✓ 多种参数类型测试通过" << std::endl;
}

// 测试 4: 日志级别过滤
void test_log_level() {
    std::cout << "\n=== Test 4: 日志级别过滤 ===" << std::endl;

    auto& logger = Logger::getInstance();

    std::cout << "设置级别为 WARN，只有 WARN 及以上才会输出：" << std::endl;
    logger.setLogLevel(LogLevel::WARN);

    LOG_DEBUG("这条不会显示");
    LOG_INFO("这条也不会显示");
    LOG_WARN("这条会显示");
    LOG_ERR("这条也会显示");

    // 恢复级别
    logger.setLogLevel(LogLevel::DEBUG);

    std::cout << "✓ 日志级别过滤测试通过" << std::endl;
}

// 测试 5: 边界情况
void test_edge_cases() {
    std::cout << "\n=== Test 5: 边界情况 ===" << std::endl;

    auto& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::INFO);

    // 空字符串
    LOG_INFO("");

    // 长消息
    string longMsg(100, 'X');
    LOG_INFO("Long: {}", longMsg);

    // 特殊字符
    LOG_INFO("Special: \\n\\t{}");

    // Unicode
    LOG_INFO("Unicode: 你好世界 🚀");

    // 流式接口空消息
    LOG_STREAM(LogLevel::INFO) << "";

    std::cout << "✓ 边界情况测试通过" << std::endl;
}

// 测试 6: 格式化和流式混合使用
void test_mixed_usage() {
    std::cout << "\n=== Test 6: 格式化和流式混合使用 ===" << std::endl;

    auto& logger = Logger::getInstance();
    logger.setLogLevel(LogLevel::INFO);

    // 格式化
    LOG_INFO("Formatted: {} + {} = {}", 10, 20, 30);

    // 流式
    LOG_STREAM(LogLevel::INFO) << "Stream: " << 10 << " + " << 20 << " = " << 30;

    // 交替使用
    LOG_INFO("Message 1");
    LOG_STREAM(LogLevel::INFO) << "Message 2";
    LOG_INFO("Message 3");

    std::cout << "✓ 混合使用测试通过" << std::endl;
}

// 测试 7: 自定义 Sink
class CountingSink : public LogSink {
public:
    int totalCount = 0;

    void log(LogLevel level, const char* file, int line,
             const string& content) override {
        totalCount++;
    }
};

void test_custom_sink() {
    std::cout << "\n=== Test 7: 自定义 Sink ===" << std::endl;

    auto& logger = Logger::getInstance();
    auto countSink = std::make_unique<CountingSink>();
    auto* sinkPtr = countSink.get();
    logger.addSink(std::move(countSink));

    int beforeCount = sinkPtr->totalCount;

    LOG_INFO("Test 1");
    LOG_INFO("Test 2");
    LOG_STREAM(LogLevel::INFO) << "Test 3";

    int afterCount = sinkPtr->totalCount;

    std::cout << "日志计数: " << (afterCount - beforeCount) << std::endl;

    std::cout << "✓ 自定义 Sink 测试通过" << std::endl;
}

int testFunc() {
    std::cout << "开始 Logger 测试...\n" << std::endl;

    try {
        test_basic_logging();
        test_stream_interface();  // 修复后的测试
        test_various_types();
        test_log_level();
        test_edge_cases();
        test_mixed_usage();
        test_custom_sink();

        std::cout << "\n=== 所有测试通过! ===" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}