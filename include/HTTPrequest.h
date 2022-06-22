//
// Created by lmy on 2022/6/21.
//

#ifndef MYWS_HTTPREQUEST_H
#define MYWS_HTTPREQUEST_H

#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <buffer.h>
#include <string>
#include <regex>

//处理HTTP请求
class HTTPrequest {
public:

    HTTPrequest();

    ~HTTPrequest() = default;

    void init();

    //有限状态机解析HTTP请求
    bool parse(Buffer &buffer);

    //获取HTTP信息
    std::string path() const;

    std::string &path();

    std::string method() const;

    std::string version() const;

    std::string getPost(const std::string &key) const;

    std::string getPost(const char *key) const;

    bool isKeepAlive() const;

private:

    //解析请求行
    bool parseRequestLine(const std::string &line);

    //解析请求字段
    void parseRequestHeader(const std::string &line);

    //解析请求体
    void parseRequestBody(const std::string &line);

    static int convertHex(char ch);

    //提取请求路径
    void parsePath();

    void parsePost();

    enum PARSE_STATE : int;
    enum HTTP_CODE : int;

    PARSE_STATE m_state;

    std::string m_method, m_path, m_version, m_body;

    //存储HTTP请求字段内容
    std::unordered_map<std::string, std::string> m_header;
    std::unordered_map<std::string, std::string> m_post;

    static const std::unordered_set<std::string> DEFAULT_HTML;
};

#endif //MYWS_HTTPREQUEST_H
