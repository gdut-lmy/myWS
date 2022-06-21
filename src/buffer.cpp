//
// Created by lmy on 2022/6/14.
//

#include "buffer.h"


Buffer::Buffer(int initBufferSize) : buffer_(initBufferSize), readPos_(0), writePos_(0) {

}

size_t Buffer::writeableBytes() const {
    return buffer_.size() - writePos_;
}

size_t Buffer::readableBytes() const {
    return writePos_ - readPos_;
}

size_t Buffer::readBytes() const {
    return readPos_;
}

const char *Buffer::curReadPtr() const {
    return BeginPtr_() + readPos_;
}

const char *Buffer::curWritePtrConst() const {
    return BeginPtr_() + writePos_;
}

char *Buffer::curWritePtr() {
    return BeginPtr_() + writePos_;
}

char *Buffer::BeginPtr_() {
    return &*buffer_.begin();
}

const char *Buffer::BeginPtr_() const {
    return &*buffer_.begin();
}

std::string Buffer::AlltoStr() {
    std::string str(curReadPtr(), readableBytes());
    initPtr();
    return str;
}

ssize_t Buffer::readFd(int fd, int *Errno) {
    char buff[65535];//临时缓冲区
    struct iovec iov[2];
    const size_t writeable = writeableBytes();

    iov[0].iov_base = BeginPtr_() + writePos_;
    iov[0].iov_len = writeable;
    iov[1].iov_base = buff;
    iov[1].iov_len = sizeof(buff);

    const ssize_t len = readv(fd, iov, 2);
    if (len < 0) {
        *Errno = errno;
    } else if (static_cast<size_t>(len) <= writeable) {
        writePos_ += len;
    } else {//如果len超出了writeable，就将临时缓冲区的数据append到buffer中(调整buffer空间再copy)
        writePos_ = buffer_.size();
        append(buff, len - writeable);
    }

    return len;
}

ssize_t Buffer::writeFd(int fd, int *Errno) {
    size_t readSize = readableBytes();
    ssize_t len = write(fd, curReadPtr(), readSize);
    if (len < 0) {
        *Errno = errno;
        return len;
    }
    readPos_ += len;

    return len;
}

void Buffer::append(const char *str, size_t len) {
    assert(str);
    ensureWriteable(len);
    std::copy(str, str + len, curWritePtr());
    updateWritePtr(len);
}

void Buffer::append(const std::string &str) {
    append(str.data(), str.length());
}

void Buffer::append(const void *data, size_t len) {
    assert(data);
    append(static_cast<const char *>(data), len);
}

void Buffer::append(const Buffer &buffer) {
    append(buffer.curReadPtr(), buffer.readableBytes());
}

void Buffer::ensureWriteable(size_t len) {

    if (writeableBytes() < len) {
        allocateSpace(len);
    }
    assert(writeableBytes() >= len);
}

void Buffer::allocateSpace(size_t len) {
    if (writeableBytes() + readableBytes() < len) {
        buffer_.resize(writePos_ + len + 1);
    } else {
        size_t readable = readableBytes();
        std::copy(BeginPtr_() + readPos_, BeginPtr_() + writePos_, BeginPtr_());
        readPos_ = 0;
        writePos_ = readable;
        assert(readable == readableBytes());
    }
}

void Buffer::updateReadPtr(size_t len) {
    assert((len <= readableBytes()));
    readPos_ += len;
}

void Buffer::updateReadPtrUntilEnd(const char *end) {
    assert(end >= curReadPtr());
    updateReadPtr(end - curReadPtr());
}

void Buffer::updateWritePtr(size_t len) {

    assert((len <= writeableBytes()));
    writePos_ += len;
}

void Buffer::initPtr() {
    bzero(&buffer_[0], buffer_.size());
    readPos_ = 0;
    writePos_ = 0;
}

Buffer::~Buffer() {

}


std::ostream &operator<<(std::ostream &out, const Buffer &buffer) {

    out << buffer.readPos_ << " " << buffer.writePos_ << "\n";

    for (int i = buffer.readPos_; i <= buffer.writePos_; ++i) {
        out << buffer.buffer_[i];
    }

    return out;

}
