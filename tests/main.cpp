//
// Created by lmy on 2022/6/23.
//

#include "webserver.h"

int main(int argc, char *argv[]) {
    WebServer server(argv[1], atoi(argv[2]), 3, 60000, false, 4);
    server.run();
}