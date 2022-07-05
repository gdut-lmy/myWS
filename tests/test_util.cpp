//
// Created by lmy on 2022/7/5.
//

#include "util.h"

my::Logger::ptr g_logger =MY_LOG_ROOT();

void test_assert(){
    MY_LOG_INFO(g_logger) << my::BacktraceToString(10);
}

int main(int argc, char *argv[]) {

    test_assert();

    return 0;
}
