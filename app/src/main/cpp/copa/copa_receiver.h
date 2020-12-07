//
// Created by ahmad.hassan on 11/16/2020.
//

#ifndef CONGESTION_COPA_RECEIVER_H
#define CONGESTION_COPA_RECEIVER_H

#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <thread>
#include <fstream>
#include <csignal>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <jni.h>

#include "tcp_header.h"
#include "udp_socket.h"

#define BUFFER_SIZE 15000


class COPA_RECEIVER {
public:
    int port;
    UDPSocket sender_socket;
    void echo_packets(UDPSocket &socket);

    COPA_RECEIVER(int p) {
        port = p;
        sender_socket.bindsocket(port);
    }
};


#endif //CONGESTION_COPA_RECEIVER_H
