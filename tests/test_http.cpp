//
// Created by lmy on 2022/6/21.
//

#include "HTTPrequest.h"


int main() {
    Buffer buffer;
    std::string s1 = "POST / HTTP/1.1\r\n"
                     "Host: www.baidu.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.6)\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: 40\r\n"
                     "Connection: Keep-Alive\r\n";
    buffer.append(s1);

    HTTPrequest HP;

    HP.parse(buffer);

    std::cout << HP.method() << ' ' << HP.path() << ' ' << HP.version() << ' ' << std::endl;
}