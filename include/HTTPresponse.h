//
// Created by lmy on 2022/6/22.
//

#ifndef MYWS_HTTPRESPONSE_H
#define MYWS_HTTPRESPONSE_H

#include "buffer.h"
#include <unordered_map>
#include <iostream>
#include <fcntl.h>  //open
#include <unistd.h> //close
#include <sys/stat.h> //stat
#include <sys/mman.h> //mmap,munmap
#include <cassert>

//服务端对HTTP请求做出的响应
class HTTPresponse {
public:
    HTTPresponse();

    ~HTTPresponse();

    void init(const std::string& srcDir, std::string &path, bool keepAlive, int code = -1);

    void makeResponse(Buffer &buffer);

    void errorContent(Buffer &buffer, const std::string &message) const;

    //解除映射关系
    void unmapFile();

    char *file();

    size_t fileLength() const;

    int getCode() const;

private:

    void addStateLine(Buffer &buffer);

    void addResponseHeader(Buffer &buffer);

    void addResponseContent(Buffer &buffer);

    void errorHTML();

    std::string getFileType();

private:

    //状态码
    int m_code;

    //标志连接是否存活
    bool isKeepAlive;

    std::string m_path;

    std::string m_srcDir;

    char *m_mmFile;

    struct stat m_mmFileState;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    static const std::unordered_map<int, std::string> CODE_STATUS;
    static const std::unordered_map<int, std::string> CODE_PATH;

};

#endif //MYWS_HTTPRESPONSE_H
