#include <iostream>


#include "buffer.h"
#include "timer.h"

#include "fcntl.h"

void fun1() {
    std::cout << "fun1\n";
}


void fun2() {
    std::cout << "fun2\n";
}

int main() {


    TimerManager timer;


    timer.addTimer(2000, fun2);
    timer.addTimer(3000, fun1);
    while (1) {
        usleep(200);
        timer.tick();
    }

    return 0;
}