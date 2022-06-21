//
// Created by lmy on 2022/6/14.
//

#ifndef MYWS_BUFFER_H
#define MYWS_BUFFER_H

#include <vector>
#include <unistd.h>
#include <sys/uio.h>
#include <iostream>
#include <string>
#include <cstring>
#include <cassert>
#include <atomic>


class Buffer {
public:
    Buffer(int initBufferSize = 1024);


    //缓存区中还可以读取的字节数
    size_t writeableBytes() const;

    //缓存区中还可以写入的字节数
    size_t readableBytes() const;

    //缓存区中已经读取的字节数
    size_t readBytes() const;

    //获取当前读指针
    const char *curReadPtr() const;

    //获取当前写指针
    const char *curWritePtrConst() const;

    char *curWritePtr();


    //将缓冲区的数据转化为字符串
    std::string AlltoStr();

    ssize_t readFd(int fd, int *Errno);

    ssize_t writeFd(int fd, int *Errno);


    void append(const char *str, size_t len);

    void append(const std::string &str);

    void append(const void *data, size_t len);

    void append(const Buffer &buffer);


    void ensureWriteable(size_t len);


    void updateReadPtr(size_t len);

    void updateReadPtrUntilEnd(const char *end);

    void updateWritePtr(size_t len);

    void initPtr();

    ~Buffer();


private:
    friend std::ostream &operator<<(std::ostream &out, const Buffer &buffer);

    //返回指向缓冲区初始位置的指针
    char *BeginPtr_();

    const char *BeginPtr_() const;

    //缓冲区扩容
    void allocateSpace(size_t len);

private:
    //存储缓冲区数据的实体
    std::vector<char> buffer_;
    //读指针，指示读到哪里了
    std::atomic<std::size_t> readPos_;
    //写指针，指示写到哪里了
    std::atomic<std::size_t> writePos_;
};

std::ostream &operator<<(std::ostream &out, const Buffer &buffer);

#endif //MYWS_BUFFER_H
