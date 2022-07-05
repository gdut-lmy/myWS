//
// Created by lmy on 2022/6/21.
//

#include "HTTPconnection.h"
#include "epoller.h"



int main(int argc, char *argv[]) {

    const char *ip = argv[1];
    int port = atoi(argv[2]);


    std::string s1 = "POST / HTTP/1.1\r\n"
                     "Host: www.baidu.com\r\n"
                     "User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.6)\r\n"
                     "Content-Type: application/x-www-form-urlencoded\r\n"
                     "Content-Length: 40\r\n"
                     "Connection: Keep-Alive\r\n";


    auto *users = new HTTPconnection[100];
    Epoller epoller;

    int socketFd = socket(PF_INET, SOCK_STREAM, 0);
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int ret = bind(socketFd, (struct sockaddr *) &address, sizeof(address));
    assert(ret > 0);

    ret = listen(socketFd, 5);
    assert(ret > 0);




}