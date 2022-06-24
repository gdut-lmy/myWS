//
// Created by lmy on 2022/6/24.
//

#include "log.h"

std::string my::LogLevel::levelToString(my::LogLevel::Level level) {

    std::string res;
    switch (level) {
        case DEBUG:
            res = "DEBUG";
            break;
        case UNKNOWN:
            res = "UNKNOWN";
            break;
        case INFO:
            res = "INFO";
            break;
        case WARN:
            res = "WARN";
            break;
        case ERROR:
            res = "ERROR";
            break;
        case FATAL:
            res = "FATAL";
            break;
    }
    return res;
}
