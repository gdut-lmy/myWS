//
// Created by lmy on 2022/6/23.
//

#include "HTTPconnection.h"

const char *HTTPconnection::srcDir;
std::atomic<int> HTTPconnection::userCount;
bool HTTPconnection::isET;

HTTPconnection::HTTPconnection() {
    m_fd = -1;
    m_addr = {0};
    m_close = true;
}

HTTPconnection::~HTTPconnection() {
    closeHTTPConn();
}

void HTTPconnection::initHTTPConn(int socketFd, const sockaddr_in &addr) {
    assert(socketFd > 0);
    userCount++;
    m_addr = addr;
    m_fd = socketFd;
    m_writeBuffer.initPtr();
    m_readBuffer.initPtr();
    m_close = false;

}

ssize_t HTTPconnection::readBuffer(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = m_readBuffer.readFd(m_fd, saveErrno);
        if (len <= 0) {
            break;
        }
    } while (isET);
    return len;
}

ssize_t HTTPconnection::writeBuffer(int *saveErrno) {
    ssize_t len = -1;
    do {
        len = writev(m_fd, m_iov, m_iovCnt);
        if (len <= 0) {
            *saveErrno = errno;
            break;
        }
        if (m_iov[0].iov_len + m_iov[1].iov_len == 0) {
            break;
        } else if (static_cast<size_t>(len) > m_iov[0].iov_len) {
            m_iov[1].iov_base = (uint8_t *) m_iov[1].iov_base + (len - m_iov[0].iov_len);
            m_iov[1].iov_len -= (len - m_iov[0].iov_len);
            if (m_iov[0].iov_len) {
                m_writeBuffer.initPtr();
                m_iov[0].iov_len = 0;
            }
        } else {
            m_iov[0].iov_base = (uint8_t *) m_iov[0].iov_base + len;
            m_iov[0].iov_len -= len;
            m_writeBuffer.updateReadPtr(len);
        }
    } while (isET || writeBytes() > 10240);
    return len;
}

void HTTPconnection::closeHTTPConn() {
    m_response.unmapFile();
    if (!m_close) {
        m_close = true;
        userCount--;
        close(m_fd);
    }

}

bool HTTPconnection::handleHTTPConn() {
    m_request.init();
    if (m_readBuffer.readableBytes() <= 0) {
        return false;
    } else if (m_request.parse(m_readBuffer)) {
        m_response.init(srcDir, m_request.path(), m_request.isKeepAlive(), 200);
    } else {
        m_response.init(srcDir, m_request.path(), false, 400);
    }

    m_response.makeResponse(m_writeBuffer);
    m_iov[0].iov_base = const_cast<char *>(m_writeBuffer.curReadPtr());
    m_iov[0].iov_len = m_writeBuffer.readableBytes();
    m_iovCnt = 1;

    if (m_response.fileLength() > 0 && m_response.file()) {
        m_iov[1].iov_base = m_response.file();
        m_iov[1].iov_len = m_response.fileLength();
        m_iovCnt = 2;
    }
    return true;
}

const char *HTTPconnection::getIP() const {
    return inet_ntoa(m_addr.sin_addr);
}

int HTTPconnection::getPort() const {
    return m_addr.sin_port;
}

int HTTPconnection::getFd() const {
    return m_fd;
}

sockaddr_in HTTPconnection::getAddr() const {
    return m_addr;
}
