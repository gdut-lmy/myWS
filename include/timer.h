//
// Created by lmy on 2022/6/20.
//

#ifndef MYWS_TIMER_H
#define MYWS_TIMER_H


#include<queue>
#include<deque>
#include<unordered_map>
#include<ctime>
#include<chrono>
#include<functional>
#include<memory>
#include <mutex>
#include<chrono>
#include<queue>
#include <iostream>
#include<functional>
#include<mutex>
#include <utility>


using TimeOutFuction = std::function<void()>;
using MS = std::chrono::milliseconds;
using Clock = std::chrono::high_resolution_clock;
using TimePoint = Clock::time_point;

class Timer {
public:
    Timer(const TimePoint &t, TimeOutFuction timeOutFun) : expireTime_(t),
                                                           timeOutFuction_(std::forward<TimeOutFuction>(timeOutFun)),
                                                           used_(true) {};

    void setUsed(bool used) { used_ = used; };

    bool isUsed() const { return used_; };

    void runTimeOutFunction() const { timeOutFuction_(); };

    TimePoint getExpireTime() const { return expireTime_; };

    bool operator<(const Timer &a) {
        return (expireTime_ < a.getExpireTime());
    }

private:
    TimePoint expireTime_;
    TimeOutFuction timeOutFuction_;
    bool used_;
};

struct TimerCmp {
    bool operator()(const std::shared_ptr<Timer> &a, const std::shared_ptr<Timer> &b) {
        return (a->getExpireTime() > b->getExpireTime());
    }
};

class TimerManager {
public:
    TimerManager() : nowTime_(Clock::now()) {};

    ~TimerManager() = default;

    void updateTime() { nowTime_ = Clock::now(); };

    std::shared_ptr<Timer> addTimer(const int &time, const TimeOutFuction &timeOutFun);   //返回引用 还是指针 是个问题
    void delTimer(const std::shared_ptr<Timer> &timer); //因为优先队列只能删除顶部，使用惰性删除，减少开销,真正删除在tick()和getExpireTime()
    void tick();        //心跳函数
    int getExpireTime();  //获取超时时间


private:
    std::priority_queue<std::shared_ptr<Timer>, std::vector<std::shared_ptr<Timer>>, TimerCmp> mangerQueue_;  //Timer重载<,生成最小堆
    TimePoint nowTime_;
    std::mutex lock_;
};


#endif //MYWS_TIMER_H
