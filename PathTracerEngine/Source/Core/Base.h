#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#define STB_IMAGE_IMPLEMENTATION

#include <iostream>
#include <string_view>
#include <cstdint>
#include <vector>
#include <functional>
#include <filesystem>
#include <type_traits>
#include <concepts>

#if defined(_WIN32)
#include <windows.h>
#endif

namespace Log {

    enum class LogLevel : uint8_t {
        Info,
        Warning,
        Error,
        Critical
    };

#if defined(_WIN32)

    inline void SetColor(LogLevel level) {
        HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

        WORD color = 0;
        switch (level) {
            case LogLevel::Info: 
                color = FOREGROUND_GREEN | FOREGROUND_INTENSITY; 
                break;
            case LogLevel::Warning:  
                color = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY; 
                break;
            case LogLevel::Error:    
                color = FOREGROUND_RED | FOREGROUND_INTENSITY; 
                break;
            case LogLevel::Critical: 
                color = BACKGROUND_RED | FOREGROUND_RED | FOREGROUND_INTENSITY;
                break;
        }

        SetConsoleTextAttribute(hConsole, color);
    }

    inline void ResetColor() {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),
            FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
    }

#else
    // ANSI escape codes (Linux / macOS)
    constexpr const char* ColorInfo = "\033[32m";
    constexpr const char* ColorWarning = "\033[33m";
    constexpr const char* ColorError = "\033[31m";
    constexpr const char* ColorCritical = "\033[1;31m";
    constexpr const char* ColorReset = "\033[0m";
#endif

    inline void Print(LogLevel level, std::string_view msg) {
#if defined(_WIN32)
        SetColor(level);
#else
        switch (level) {
            case LogLevel::Info:     
                std::cout << ColorInfo; 
                break;
            case LogLevel::Warning:  
                std::cout << ColorWarning; 
                break;
            case LogLevel::Error:    
                std::cout << ColorError; 
                break;
            case LogLevel::Critical: 
                std::cout << ColorCritical; 
                break;
        }
#endif

        switch (level) {
            case LogLevel::Info:     
                std::cout << "[INFO] "; 
                break;
            case LogLevel::Warning:  
                std::cout << "[WARNING] "; 
                break;
            case LogLevel::Error:    
                std::cout << "[ERROR] "; 
                break;
            case LogLevel::Critical: 
                std::cout << "[CRITICAL] "; 
                break;
        }

        std::cout << msg << '\n';

#if defined(_WIN32)
        ResetColor();
#else
        std::cout << ColorReset;
#endif
    }

    inline void Assert(bool val, std::string_view msg) {
        if (!val) {
            Print(LogLevel::Critical, msg);
#if defined(DEBUG)
            __debugbreak();
#endif
        }
    }

    template <typename Func>
    inline void AssertAndIfFails(bool val, std::string_view msg, const Func&& OnFail) {
        if (!val) {
            Print(LogLevel::Critical, msg);
            OnFail();
        }
    }
}

#define PT_LOG_INFO(msg)     Log::Print(Log::LogLevel::Info, msg)
#define PT_LOG_WARN(msg)     Log::Print(Log::LogLevel::Warning, msg)
#define PT_LOG_ERROR(msg)    Log::Print(Log::LogLevel::Error, msg)
#define PT_LOG_CRITICAL(msg) Log::Print(Log::LogLevel::Critical, msg)

#define PT_ASSERT(val, msg)  Log::Assert(val, msg)
#define PT_ASSERT_AND_IF_FAILS(val, msg, onFail) Log::AssertAndIfFails(val, msg, onFail)
