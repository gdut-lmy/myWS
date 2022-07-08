//
// Created by lmy on 2022/7/5.
//

#include "log.h"

int main(int argc, char *argv[]) {

    auto logger = my::LogMgr::GetInstance()->getLogger("system");
    MY_LOG_ERROR(logger)<<"system";
    return 0;
}