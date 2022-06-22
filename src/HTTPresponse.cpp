//
// Created by lmy on 2022/6/22.
//

#include "HTTPresponse.h"

#include <utility>

const std::unordered_map<std::string, std::string> HTTPresponse::SUFFIX_TYPE = {
        {".html",  "text/html"},
        {".xml",   "text/xml"},
        {".xhtml", "application/xhtml+xml"},
        {".txt",   "text/plain"},
        {".rtf",   "application/rtf"},
        {".pdf",   "application/pdf"},
        {".word",  "application/nsword"},
        {".png",   "image/png"},
        {".gif",   "image/gif"},
        {".jpg",   "image/jpeg"},
        {".jpeg",  "image/jpeg"},
        {".au",    "audio/basic"},
        {".mpeg",  "video/mpeg"},
        {".mpg",   "video/mpeg"},
        {".avi",   "video/x-msvideo"},
        {".gz",    "application/x-gzip"},
        {".tar",   "application/x-tar"},
        {".css",   "text/css "},
        {".js",    "text/javascript "},
};

const std::unordered_map<int, std::string> HTTPresponse::CODE_STATUS = {
        {200, "OK"},
        {400, "Bad Request"},
        {403, "Forbidden"},
        {404, "Not Found"},
};

const std::unordered_map<int, std::string> HTTPresponse::CODE_PATH = {
        {400, "/400.html"},
        {403, "/403.html"},
        {404, "/404.html"},
};

HTTPresponse::HTTPresponse() {

    m_code = -1;
    m_path = m_srcDir = "";
    isKeepAlive = false;
    m_mmFile = nullptr;
    m_mmFileState = {0};

}

void HTTPresponse::init(const std::string &srcDir, std::string &path, bool keepAlive, int code) {
    assert(!srcDir.empty());
    if (m_mmFile) {
        unmapFile();
    }
    m_code = code;
    this->isKeepAlive = keepAlive;
    m_srcDir = srcDir;
    m_path = path;
    m_mmFile = nullptr;
    m_mmFileState = {0};
}

HTTPresponse::~HTTPresponse() {
    unmapFile();
}

void HTTPresponse::unmapFile() {
    if (m_mmFile) {
        munmap(m_mmFile, m_mmFileState.st_size);
        m_mmFile = nullptr;
    }
}

char *HTTPresponse::file() {
    return m_mmFile;
}

size_t HTTPresponse::fileLength() const {
    return m_mmFileState.st_size;
}

std::string HTTPresponse::getFileType() {
    /* 判断文件类型 */
    std::string::size_type idx = m_path.find_last_of('.');
    if (idx == std::string::npos) {
        return "text/plain";
    }
    std::string suffix = m_path.substr(idx);
    if (SUFFIX_TYPE.count(suffix) == 1) {
        return SUFFIX_TYPE.find(suffix)->second;
    }
    return "text/plain";
}


void HTTPresponse::makeResponse(Buffer &buffer) {
    if (stat((m_srcDir + m_path).data(), &m_mmFileState) < 0 || S_ISDIR(m_mmFileState.st_mode)) {
        m_code = 404;
    } else if (!(m_mmFileState.st_mode & S_IROTH)) {
        m_code = 403;
    } else if (m_code == -1) {
        m_code = 200;
    }

    errorHTML();
    addStateLine(buffer);
    addResponseHeader(buffer);
    addResponseContent(buffer);

}


void HTTPresponse::addStateLine(Buffer &buffer) {
    std::string status;
    if (CODE_STATUS.count(m_code) == 1) {
        status = CODE_STATUS.find(m_code)->second;
    } else {
        m_code = 400;
        status = CODE_STATUS.find(400)->second;
    }
    buffer.append("HTTP/1.1" + std::to_string(m_code) + " " + status + "\r\n");
}

void HTTPresponse::addResponseHeader(Buffer &buffer) {
    buffer.append("Connection");
    if (isKeepAlive) {
        buffer.append("keep-alive\r\n");
        buffer.append("keep-alive: max=6, timeout=120\r\n");
    } else {
        buffer.append("close\r\n");
    }
    buffer.append("Content-type: " + getFileType() + "\r\n");
}

void HTTPresponse::addResponseContent(Buffer &buffer) {
    int srcFd = open((m_srcDir + m_path).data(), O_RDONLY);
    if (srcFd < 0) {
        errorContent(buffer, "File NotFound!");
        return;
    }

    void *ret = mmap(nullptr, m_mmFileState.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
    if (ret == (void *) -1) {
        errorContent(buffer, "File NotFound!");
        return;
    }
    m_mmFile = (char *) ret;
    close(srcFd);
    buffer.append("Content-length: " + std::to_string(m_mmFileState.st_size) + "\r\n\r\n");
}


void HTTPresponse::errorHTML() {
    if (CODE_PATH.count(m_code) == 1) {
        m_path = CODE_PATH.find(m_code)->second;
        stat((m_srcDir + m_path).data(), &m_mmFileState);
    }
}

// The  munmap()  system  call deletes the mappings for the specified address range,
//       and causes further references to addresses within the range to  generate  invalid
//       memory references.  The region is also automatically unmapped when the process is
//       terminated.  On the other hand, closing the file descriptor does  not  unmap  the
//       region.

void HTTPresponse::errorContent(Buffer &buffer, const std::string &message) const {
    std::string body;
    std::string status;
    body += "<html><title>Error</title>";
    body += "<body bgcolor=\"ffffff\">";
    if (CODE_STATUS.count(m_code) == 1) {
        status = CODE_STATUS.find(m_code)->second;
    } else {
        status = "Bad Request";
    }
    body += std::to_string(m_code) + " : " + status + "\n";
    body += "<p>" + message + "</p>";
    body += "<hr><em>TinyWebServer</em></body></html>";

    buffer.append("Content-length: " + std::to_string(body.size()) + "\r\n\r\n");
    buffer.append(body);
}

int HTTPresponse::getCode() const {
    return m_code;
}
