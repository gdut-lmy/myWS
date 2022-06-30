//
// Created by lmy on 2022/6/30.
//

#include "mutex.h"

namespace my {

    RWMutex rwlock;

    void fun() {
        RWMutex::ReadLock lock(rwlock);
    };

}