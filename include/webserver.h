//
// Created by lmy on 2022/6/23.
//

#ifndef MYWS_WEBSERVER_H
#define MYWS_WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <cassert>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "timer.h"
#include "threadPool.h"
#include "HTTPconnection.h"

class WebServer {
public:
    WebServer(const char* ip,int port, int trigMode, int timeoutMS, bool optLinger, int threadNum);

    ~WebServer();

    void run();

private:
    bool initSocket();

    void initEpollMode(int trigMode);

    void addClientConnection(int fd, sockaddr_in addr);

    void closeConnection(HTTPconnection *client);

    void handleListen();

    void handleWrite(HTTPconnection *client);

    void handleRead(HTTPconnection *client);

    void onRead(HTTPconnection *client);

    void onWrite(HTTPconnection *client);

    void onProcess(HTTPconnection *client);

    void sendError(int fd, const char *info);

    void extentTime(HTTPconnection *client);

    static const int MAX_FD = 65536;

    static int setFdNonblock(int fd);

    int port_;
    int timeoutMS_;  /* 毫秒MS,定时器的默认过期时间 */
    bool isClose_;
    int listenFd_;
    bool openLinger_;
    const char *m_ip;
    char *srcDir_;//需要获取的路径

    uint32_t listenEvent_;
    uint32_t connectionEvent_;

    std::shared_ptr<TimerManager> timer_;
    std::shared_ptr<my::threadPool> threadpool_;
    std::shared_ptr<Epoller> epoller_;
    std::unordered_map<int, HTTPconnection> users_;


};

#endif //MYWS_WEBSERVER_H
