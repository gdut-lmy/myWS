//
// Created by lmy on 2022/6/23.
//

#ifndef MYWS_HTTPCONNECTION_H
#define MYWS_HTTPCONNECTION_H

#include<arpa/inet.h> //sockaddr_in
#include<sys/uio.h> //readv/writev
#include<iostream>
#include<sys/types.h>
#include<cassert>
#include <utility>

#include "buffer.h"
#include "HTTPrequest.h"
#include "HTTPresponse.h"
#include "timer.h"

class HTTPconnection {
public:
    HTTPconnection();

    ~HTTPconnection();

    void initHTTPConn(int socketFd, const sockaddr_in &addr);

    //每个连接中定义的对缓冲区的读写接口
    ssize_t readBuffer(int *saveErrno);

    ssize_t writeBuffer(int *saveErrno);

    //关闭HTTP连接的接口
    void closeHTTPConn();

    //定义处理该HTTP连接的接口，主要分为request的解析和response的生成
    bool handleHTTPConn();

    //其他方法
    const char *getIP() const;

    int getPort() const;

    int getFd() const;

    sockaddr_in getAddr() const;

    int writeBytes() {
        return m_iov[1].iov_len + m_iov[0].iov_len;
    }

    bool isKeepAlive() const {
        return m_request.isKeepAlive();
    }

    void setTimer(std::shared_ptr<Timer> timer) {
        m_timer = std::move(timer);
    }

    std::shared_ptr<Timer> getTimer() const {
        return m_timer;
    }

public:
    static bool isET;
    static const char *srcDir;
    static std::atomic<int> userCount;

private:
    int m_fd;//HTTP连接对应的文件描述符
    struct sockaddr_in m_addr;//获取IP和PORT
    bool m_close;

    //writev
    int m_iovCnt;
    struct iovec m_iov[2];

    std::shared_ptr<Timer> m_timer;

    Buffer m_readBuffer;
    Buffer m_writeBuffer;

    HTTPrequest m_request;
    HTTPresponse m_response;
};


#endif //MYWS_HTTPCONNECTION_H
