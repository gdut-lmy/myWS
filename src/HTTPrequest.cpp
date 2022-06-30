//
// Created by lmy on 2022/6/21.
//

#include "HTTPrequest.h"

const std::unordered_set<std::string> HTTPrequest::DEFAULT_HTML{
        "/index", "/welcome", "/video", "/picture"};


enum HTTPrequest::PARSE_STATE : int {
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH
};

enum HTTPrequest::HTTP_CODE : int {
    NO_REQUEST = 0,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDENT_REQUEST,
    FILE_REQUEST,
    INTERNAL_ERROR,
    CLOSED_CONNECTION
};

HTTPrequest::HTTPrequest() {
    init();
}


void HTTPrequest::init() {
    m_method = m_path = m_version = m_body = "";
    m_state = REQUEST_LINE;
    m_header.clear();
    m_post.clear();
}

bool HTTPrequest::isKeepAlive() const {
    if (m_header.count("Connection") == 1) {
        return m_header.find("Connection")->second == "keep-alive" && m_version == "1.1";
    }
    return false;
}

bool HTTPrequest::parse(Buffer &buffer) {
    const char CRLF[] = "\r\n";
    if (buffer.readableBytes() == 0) {
        return false;
    }
    //有限状态机解析
    while (buffer.readableBytes() && m_state != FINISH) {
        const char *lineEnd = std::search(buffer.curReadPtr(), buffer.curWritePtrConst(), CRLF, CRLF + 2);
        std::string line(buffer.curReadPtr(), lineEnd);
        switch (m_state) {
            case REQUEST_LINE:
                if (!parseRequestLine(line)) {
                    return false;
                }
                parsePath();
                break;
            case HEADERS:
                parseRequestHeader(line);
                if (buffer.readableBytes() <= 2) {
                    m_state = FINISH;
                }
                break;
            case BODY:
                parseRequestBody(line);
                break;
            default:
                break;
        }
        if (lineEnd == buffer.curWritePtr()) {
            break;
        }
        buffer.updateReadPtrUntilEnd(lineEnd + 2);
    }
    return true;
}

bool HTTPrequest::parseRequestLine(const std::string &line) {
    //HTTP格式实例：POST /DEMOWebServices2.8/Service.asmx/CancelOrder HTTP/1.1
    //利用正则表达式得到http报文的属性
    std::regex patten("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, patten)) {
        m_method = subMatch[1];
        m_path = subMatch[2];
        m_version = subMatch[3];
        m_state = HEADERS;
        return true;
    }
    return false;
}

void HTTPrequest::parseRequestHeader(const std::string &line) {
    std::regex patten("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if (std::regex_match(line, subMatch, patten)) {
        m_header[subMatch[1]] = subMatch[2];
    } else {
        m_state = BODY;
    }


}

void HTTPrequest::parseRequestBody(const std::string &line) {
    m_body = line;
    parsePost();
    m_state = FINISH;
}


std::string HTTPrequest::path() const {
    return m_path;
}

std::string &HTTPrequest::path() {
    return m_path;
}

std::string HTTPrequest::method() const {
    return m_method;
}

std::string HTTPrequest::version() const {
    return m_version;
}

std::string HTTPrequest::getPost(const std::string &key) const {
    assert(key != " ");
    if (m_post.count(key) == 1) {
        return m_post.find(key)->second;
    }
    return "";
}

std::string HTTPrequest::getPost(const char *key) const {
    assert(key != " ");
    if (m_post.count(key) == 1) {
        return m_post.find(key)->second;
    }
    return "";
}

void HTTPrequest::parsePath() {
    if (m_path == "/") {
        m_path = "/index.html";
    } else {
        for (auto &item: DEFAULT_HTML) {
            if (item == m_path) {
                m_path += ".html";
                break;
            }
        }
    }
}

int HTTPrequest::convertHex(char ch) {
    if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
    if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
    return ch;
}

void HTTPrequest::parsePost() {
    if (m_method == "POST" && m_header["Content-Type"] == "application/x-www-form-urlencoded") {
        if (m_body.empty()) { return; }

        std::string key, value;
        int num = 0;
        int n = m_body.size();
        int i = 0, j = 0;

        for (; i < n; i++) {
            char ch = m_body[i];
            switch (ch) {
                case '=':
                    key = m_body.substr(j, i - j);
                    j = i + 1;
                    break;
                case '+':
                    m_body[i] = ' ';
                    break;
                case '%':
                    num = convertHex(m_body[i + 1]) * 16 + convertHex(m_body[i + 2]);
                    m_body[i + 2] = num % 10 + '0';
                    m_body[i + 1] = num / 10 + '0';
                    i += 2;
                    break;
                case '&':
                    value = m_body.substr(j, i - j);
                    j = i + 1;
                    m_post[key] = value;
                    break;
                default:
                    break;
            }
        }
        assert(j <= i);
        if (m_post.count(key) == 0 && j < i) {
            value = m_body.substr(j, i - j);
            m_post[key] = value;
        }
    }
}
