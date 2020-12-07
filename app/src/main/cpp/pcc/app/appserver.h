//
// Created by ahmad.hassan on 11/17/2020.
//

#ifndef CONGESTION_APPSERVER_H
#define CONGESTION_APPSERVER_H

#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <netdb.h>
#include <iostream>
#include "../src/udt.h"
#include "cc.h"
#include "test_util.h"

class AppServer {
public:
    int port = 5001;
    // Automatically start up and clean up UDT module.
    UDTUpDown _udt_;
    addrinfo hints{};
    addrinfo* res;
    static void* recvdata(void*);
    static void* monitor(void* s);
    int run();

    AppServer(int p) {

        memset(&hints, 0, sizeof(struct addrinfo));

        hints.ai_flags = AI_PASSIVE;
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        run();
    }

};



#endif //CONGESTION_APPSERVER_H
