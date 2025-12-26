#include <windows.h>
#include <iostream>
#include "Logger.h"

int testFunc();

int main() {
    // 终端显色用
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // Logger::getInstance().addSink(std::make_unique<ConsoleLogSink>());
    // Logger::getInstance().setLogLevel(LogLevel::DEBUG);
    // LOG_DEBUG("DEBUG");
    // LOG_INFO("INFO");
    // LOG_ERR("ERR");
    // LOG_WARN("WARN");
    // LOG_FATAL("FATAL");
    //
    // std::cout << "Hello, World!" << std::endl;

    testFunc();

    return 0;
}
