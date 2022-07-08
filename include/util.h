//
// Created by lmy on 2022/7/5.
//

#ifndef MYWS_UTIL_H
#define MYWS_UTIL_H


#include <cassert>
#include <string>
#include <cxxabi.h>
#include <pthread.h>
#include <unistd.h>
#include <execinfo.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <cstdio>
#include <cstdint>
#include <vector>
#include <string>
#include <iomanip>
#include <yaml-cpp/yaml.h>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "log.h"
#include "fiber.h"

namespace my {

    /**
     * @brief 返回当前线程的ID
     */
    pid_t GetThreadId();

    /**
     * @brief 返回当前协程的ID
     */
    uint32_t GetFiberId();

    /**
     * @brief 获取当前的调用栈
     * @param[out] bt 保存调用栈
     * @param[in] size 最多返回层数
     * @param[in] skip 跳过栈顶的层数
     */
    void Backtrace(std::vector<std::string> &bt, int size = 64, int skip = 1);

    /**
     * @brief 获取当前栈信息的字符串
     * @param[in] size 栈的最大层数
     * @param[in] skip 跳过栈顶的层数
     * @param[in] prefix 栈信息前输出的内容
     */
    std::string BacktraceToString(int size = 64, int skip = 2, const std::string &prefix = "");

}


#endif //MYWS_UTIL_H
