//
// Created by lmy on 2022/6/21.
//

#include "HTTPrequest.h"
#include "HTTPresponse.h"

int main() {
    Buffer qbuffer;
    Buffer pbuffer;
    std::string s1 = "POST / HTTP/1.1\r\n"
                     "Host: www.baidu.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.6)\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: 40\r\n"
                     "Connection: Keep-Alive\r\n";


    qbuffer.append(s1);

    std::cout << qbuffer;

    HTTPrequest HP;

    HTTPresponse res;

    HP.parse(qbuffer);

    res.init("/resource", HP.path(), HP.isKeepAlive(), 200);
    res.makeResponse(pbuffer);

    std::cout << pbuffer;


}