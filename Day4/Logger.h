//
// Created by Ayr on 2025/12/24.
//

#ifndef DAY4_LOGGER_H
#define DAY4_LOGGER_H

// 前景色
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

#include <string>
#include <memory>
#include <vector>
#include <format>
#include <iostream>
#include <sstream>

using std::string;
using std::unique_ptr;
using std::vector;

enum class LogLevel { DEBUG, INFO, WARN, ERR, FATAL };
constexpr const char* const LogLevelNames[] = {
    "DEBUG", "INFO", "WARN", "ERR", "FATAL"
};

constexpr const char* LogLevelColors[] = {
    COLOR_CYAN,   // DEBUG
    COLOR_GREEN,  // INFO
    COLOR_YELLOW, // WARN
    COLOR_RED,    // ERROR
    COLOR_MAGENTA // FATAL
};

class LogSink {
public:
    virtual ~LogSink() = default;
    virtual void log(LogLevel level, const char* file, int line,
        const std::string& content) = 0;
};

class ConsoleLogSink : public LogSink {
    public:
    void log(LogLevel level, const char* file, int line,
        const std::string& content) override {
        std::cout << LogLevelColors[static_cast<int>(level)]
                    << "[" << LogLevelNames[static_cast<int>(level)] << "]"
                    << "[" << file << ":" << line<< "]"
                    << COLOR_RESET
                    << content
                    << std::endl;
    }
};

class Logger {
    Logger() = default;
    ~Logger() = default;

    LogLevel gLevel = LogLevel::INFO;
    vector<unique_ptr<LogSink>> sinks;
public:
    // 单例类 禁用拷贝和显式构造函数
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger(Logger const&) = delete;
    Logger& operator=(Logger const&) = delete;

    LogLevel getLevel() const {
        return gLevel;
    }
    void setLogLevel(LogLevel level) {
        gLevel = level;
    }

    template<typename... Args>
    void log(LogLevel level, const char* file, int line,
             const string& fmt, Args&&... args) {
        if (level < gLevel) return;

        for (auto& sink : sinks) {
            sink->log(level, file, line,
                sizeof...(args) == 0 ? fmt : std::vformat(fmt, std::make_format_args(args...))
            );
        }
    }

    // 流式接口
    class LogStream {
        LogLevel gLevel;
        const char *gFile;
        int gLine;
        std::stringstream os;
    public:
        LogStream(LogLevel level, const char* file = __FILE__, int line = __LINE__)
            : gLevel(level), gFile(file), gLine(line) {}

        template<typename T>
        LogStream& operator<<(T&& value) {
            if (gLevel >= Logger::getInstance().getLevel()) os << std::forward<T>(value);
            return *this;
        }
        ~LogStream() {
            Logger::getInstance().log(gLevel,
                gFile, gLine, os.str());
        }
    };

    static LogStream stream(const LogLevel level) {
        return LogStream(level);
    }

    // 添加输出目标
    void addSink(unique_ptr<LogSink> sink) {
        sinks.push_back(std::move(sink));
    }
};

// 便利宏
#define LOG_STREAM(level) Logger::stream(level)
#define LOG_DEBUG(...) Logger::getInstance().log(LogLevel::DEBUG, \
__FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...) Logger::getInstance().log(LogLevel::INFO, \
__FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARN(...) Logger::getInstance().log(LogLevel::WARN, \
__FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERR(...) Logger::getInstance().log(LogLevel::ERR, \
__FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...) Logger::getInstance().log(LogLevel::FATAL, \
__FILE__, __LINE__, __VA_ARGS__)

#endif //DAY4_LOGGER_H