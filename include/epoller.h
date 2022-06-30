//
// Created by lmy on 2022/6/14.
//

#ifndef MYWS_EPOLLER_H
#define MYWS_EPOLLER_H

#include<sys/epoll.h> //epoll_ctl()
#include<fcntl.h> //fcntl()
#include<unistd.h> //close()
#include<cassert>
#include<vector>
#include<cerrno>

class Epoller {
public:
    explicit Epoller(int maxEvent = 1024);

    //将描述符fd加入epoll监控
    bool addFd(int fd, uint32_t events);

    //修改描述符fd对应的事件
    bool modFd(int fd, uint32_t events);

    //删除描述符fd在epoll中的监控
    bool delFd(int fd);

    int wait(int timewait = -1);

    //返回epoll事件的fd
    int getEventFd(size_t i) const;

    //返回epoll事件
    uint32_t getEvents(size_t i) const;

    ~Epoller();

private:
    int epollerFd_;
    std::vector<struct epoll_event> events_;
};


#endif //MYWS_EPOLLER_H
