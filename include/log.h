//
// Created by lmy on 2022/6/24.
//

#ifndef MYWS_LOG_H
#define MYWS_LOG_H

#include <string>
#include <iostream>
#include <vector>

namespace my {
    class LogLevel {
    public:
        enum Level {
            UNKNOWN = 0,
            DEBUG = 1,
            INFO = 2,
            WARN = 3,
            ERROR = 4,
            FATAL = 5
        };

        static std::string levelToString(LogLevel::Level level);
    };

    struct LogAppenderConfig {
        enum Type {
            Stdout = 0,
            File = 1
        };
        LogAppenderConfig::Type type;
        LogLevel::Level level;
        std::string formatter;
        std::string file;

        LogAppenderConfig() : type(Type::Stdout), level(LogLevel::UNKNOWN) {}

        bool operator==(const LogAppenderConfig &lhs) const {
            return type == lhs.type &&
                   level == lhs.level &&
                   formatter == lhs.formatter &&
                   file == lhs.file;
        }
    };

    struct LogConfig {
        std::string name;
        LogLevel::Level level;
        std::string formatter;
        std::vector<LogAppenderConfig> appender;

        LogConfig() : level(LogLevel::UNKNOWN) {}

        bool operator==(const LogConfig& lhs) const
        {
            return name == lhs.name /* &&
               level == lhs.level &&
               formatter == lhs.formatter &&
               appender == lhs.appender*/;
        }
    };


}


#endif //MYWS_LOG_H
