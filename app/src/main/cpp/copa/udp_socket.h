//
// Created by ahmad.hassan on 11/16/2020.
//

#ifndef CONGESTION_UDP_SOCKET_H
#define CONGESTION_UDP_SOCKET_H

#include <string>

#include <netinet/in.h>
#include <sys/poll.h>
#include <sys/socket.h>


class UDPSocket {
public:
	typedef sockaddr_in SockAddress;
private:
	int udp_socket;

	std::string ipaddr;
	int port;
  	int srcport;

	bool bound;
public:
	UDPSocket() : udp_socket(-1), ipaddr(), port(), srcport(), bound(false) {
		udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
	}

	int bindsocket(int port);
	ssize_t senddata(const char* data, ssize_t size, SockAddress *s_dest_addr);
	int receivedata(char* buffer, int bufsize, int timeout, SockAddress &other_addr);
};



#endif //CONGESTION_UDP_SOCKET_H
