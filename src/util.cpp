//
// Created by lmy on 2022/7/5.
//


#include "util.h"


namespace my {
    static my::Logger::ptr g_logger = MY_LOG_NAME("system");

    pid_t GetThreadId() {
        return syscall(SYS_gettid);
    }

    static std::string demangle(const char *str) {
        size_t size = 0;
        int status = 0;
        std::string rt;
        rt.resize(256);
        if (1 == sscanf(str, "%*[^(]%*[^_]%255[^)+]", &rt[0])) {
            char *v = abi::__cxa_demangle(&rt[0], nullptr, &size, &status);
            if (v) {
                std::string result(v);
                free(v);
                return result;
            }
        }
        if (1 == sscanf(str, "%255s", &rt[0])) {
            return rt;
        }
        return str;
    }

    void Backtrace(std::vector<std::string> &bt, int size, int skip) {
        void **array = (void **) malloc((sizeof(void *) * size));
        size_t s = ::backtrace(array, size);

        char **strings = backtrace_symbols(array, s);
        if (strings == NULL) {
            MY_LOG_ERROR(g_logger) << "backtrace_synbols error";
            return;
        }

        for (size_t i = skip; i < s; ++i) {
            bt.push_back(demangle(strings[i]));
        }

        free(strings);
        free(array);
    }

    uint32_t GetFiberId() {

        return my::Fiber::GetFiberId();
    }

    std::string BacktraceToString(int size, int skip, const std::string &prefix) {
        std::vector<std::string> bt;
        Backtrace(bt, size, skip);
        std::stringstream ss;
        for (auto &i: bt) {
            ss << prefix << i << std::endl;
        }
        return ss.str();
    }
}