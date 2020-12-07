//
// Created by ahmad.hassan on 11/16/2020.
//

#include "copa_receiver.h"

void COPA_RECEIVER::echo_packets(UDPSocket &socket) {
    char buff[BUFFER_SIZE];
    sockaddr_in sender_addr{};

    std::chrono::high_resolution_clock::time_point start_time_point = \
		std::chrono::high_resolution_clock::now();

    std::chrono::high_resolution_clock::time_point recv_time;

    while (true) {
        int received __attribute((unused)) = -1;
        while (received == -1) {
            received = sender_socket.receivedata(buff, BUFFER_SIZE, -1, \
					sender_addr);
            assert( received != -1 );
        }

        auto *header = (TCPHeader*)buff;
        recv_time = std::chrono::high_resolution_clock::now();
        header->receiver_timestamp = \
			std::chrono::duration_cast<std::chrono::duration<double>>(
			        recv_time - start_time_point
			                ).count()*1000; //in milliseconds

        sender_socket.senddata(buff, sizeof(TCPHeader), &sender_addr);

    }
}

