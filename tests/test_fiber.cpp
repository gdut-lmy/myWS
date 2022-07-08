//
// Created by lmy on 2022/7/7.
//

#include <fiber.h>
#include <log.h>

my::Logger::ptr g_logger = MY_LOG_ROOT();


int count = 0;

void run_in_fiber() {
    MY_LOG_INFO(g_logger) << "run_in_fiber begin";
    my::Fiber::YieldToReady();
    MY_LOG_INFO(g_logger) << "run_in_fiber end";
}

void test1() {
    //my::Fiber::EnableFiber();
    MY_LOG_INFO(g_logger) << "begin";
    my::Fiber::ptr fiber(new my::Fiber(run_in_fiber));
    fiber->swapIn();
    MY_LOG_INFO(g_logger) << "after swap in";
    fiber->swapIn();
    MY_LOG_INFO(g_logger) << "end";
}

void test2() {
    my::Fiber::ptr fiber(new my::Fiber([]() {
        while (1) {
            count++;
            my::Fiber::YieldToReady();
        }
    }));
    while (1) {
        fiber->swapIn();
        MY_LOG_DEBUG(g_logger) << count;
    }

}

int main(int argc, char **argv) {
    my::Fiber::EnableFiber();
    test1();

    //go test1;
}