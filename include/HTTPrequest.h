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

class HTTPrequest {
public:

    HTTPrequest();

    ~HTTPrequest() = default;

    void init();

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

    bool parseRequestLine(const std::string &line);

    void parseRequestHeader(const std::string &line);

    void parseRequestBody(const std::string &line);

    static int convertHex(char ch);

    void parsePath();

    void parsePost();

    enum PARSE_STATE : int;
    enum HTTP_CODE : int;

    PARSE_STATE m_state;

    std::string m_method, m_path, m_version, m_body;

    std::unordered_map<std::string, std::string> m_header;
    std::unordered_map<std::string, std::string> m_post;

    static const std::unordered_set<std::string> DEFAULT_HTML;
};

#endif //MYWS_HTTPREQUEST_H
