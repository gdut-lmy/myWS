//
// Created by lmy on 2022/7/7.
//

#ifndef MYWS_MACRO_H
#define MYWS_MACRO_H

#include <cassert>
#include <cstdlib>
#include <string>
#include "util.h"

#if defined(__GNUC__)  || defined(__llvm__)
#   define MY_LIKELY(x)       __builtin_expect(!!(x), 1)
#   define MY_UNLIKELY(x)     __builtin_expect(!!(x), 0)
#else
#   define MY_LIKELY(x)       (x)
#   define MY_UNLIKELY(x)     (x)
#endif

#define MY_ASSERT(x) \
if(MY_UNLIKELY(!(x))){          \
    MY_LOG_ERROR(MY_LOG_ROOT()) << "ASSERTION: " << #x \
        << "\nbacktrace:\n" << my::BacktraceToString(100,2,"    "); \
    assert(x);         \
    exit(1);                       \
}
#define MY_ASSERT2(x,m) \
if(MY_UNLIKELY(!(x))){                 \
    MY_LOG_ERROR(MY_LOG_ROOT()) << "ASSERTION: " << #x \
        << "\n" << m <<"\n"              \
        << "\nbacktrace:\n" << my::BacktraceToString(100,2,"    "); \
    assert(x);            \
    exit(1);                       \
}

#define MY_STATIC_ASSERT(x) \
if constexpr(!(x)){          \
    MY_LOG_ERROR(MY_LOG_ROOT()) << "ASSERTION: " << #x \
        << "\nbacktrace:\n" << my::BacktraceToString(100,2,"    "); \
    exit(1);                          \
}


#endif //MYWS_MACRO_H
