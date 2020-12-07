//
// Created by ahmad.hassan on 11/16/2020.
//
#include <arpa/inet.h>
#include <cassert>
#include <cerrno>
#include <iostream>
#include <cstring>

#include "udp_socket.h"


int UDPSocket::bindsocket(int s_port)
{
	ipaddr = "";
	port = s_port;
 	sockaddr_in addr_struct{};
	memset((char *) &addr_struct, 0, sizeof(addr_struct));

    addr_struct.sin_family = AF_INET;
    addr_struct.sin_port = htons(port);
    addr_struct.sin_addr.s_addr = htonl(INADDR_ANY);
 	if (bind(udp_socket , (struct sockaddr*)&addr_struct, sizeof(addr_struct) ) != 0){
 		std::cerr<<"Error while binding socket. Code: "<<errno<<std::endl;
 		return -1;
 	}
 	bound = true;
 	return 0;
}

// Sends data to the desired address. Returns number of bytes sent if
// successful, -1 if not.
ssize_t UDPSocket::senddata(const char* data, ssize_t size, sockaddr_in *s_dest_addr){
	sockaddr_in dest_addr{};
	memset((char *) &dest_addr, 0, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	if(s_dest_addr == nullptr){
		assert(bound); // Socket not bound to an address. Please use 'bindsocket'

	    dest_addr.sin_port = htons(port);

	    if (inet_aton(ipaddr.c_str(), &dest_addr.sin_addr) == 0)
	    {
	        std::cerr<<"inet_aton failed while sending data. Code: "<<errno<<std::endl;
	    }
	}
	else{
	    dest_addr.sin_port = ((struct sockaddr_in *)s_dest_addr)->sin_port;
	    dest_addr.sin_addr = ((struct sockaddr_in *)s_dest_addr)->sin_addr;
	}

	int res = sendto(udp_socket, data, size, 0, (struct sockaddr *) &dest_addr, sizeof(dest_addr));

	if ( res == -1 ){
		std::cerr<<"Error while sending datagram. Code: "<<errno<<std::endl;
		//assert(false);
		return -1;
	}

	return res; // no of bytes sent
}

// Modifies buffer to contain a null terminated string of the received
// data and returns the received buffer size (or -1 or 0, see below)
//
// Takes timeout in milliseconds. If timeout, returns -1 without
// changing the buffer. If data arrives before timeout, modifies buffer
// with the received data and returns the places the sender's address
// in other_addr
//
// If timeout is negative, infinite timeout will be used. If it is 0,
// function will return immediately. Timeout will be rounded up to
// kernel time granularity, and kernel scheduling delays may cause
// actual timeout to exceed what is specified
int UDPSocket::receivedata(char* buffer, int bufsize, int timeout, sockaddr_in &other_addr){
	assert(bound); // Socket not bound to an address. Please either use 'bind' or 'sendto'

	unsigned int other_len;

	struct pollfd pfds[1];
	pfds[0].fd = udp_socket;
	pfds[0].events = POLLIN;

	int poll_val = poll(pfds, 1, timeout);
	if( poll_val == 1){
		if(pfds[0].revents & POLLIN){
			other_len = sizeof(other_addr);
			int res = recvfrom(udp_socket, buffer, bufsize, 0, (struct sockaddr*) &other_addr,
							   reinterpret_cast<socklen_t *>(&other_len));
			if ( res == -1 ){
				std::cerr<<"Error while receiving datagram. Code: "<<errno<<std::endl;
			}
			buffer[res] = '\0'; //terminating null character is not added by default

			return res;
		}
		else{
			std::cerr<<"There was an error while polling. Value of event field: "<<pfds[0].revents<<std::endl;
			return -1;
		}
	}
	else if ( poll_val == 0){
		return 0; //there was a timeout
	}
	else if ( poll_val == -1 ){
		if ( errno == 4 )
			return receivedata(buffer, bufsize, timeout, other_addr); //to make gprof work
		std::cerr<<"There was an error while polling. Code: "<<errno<<std::endl;
		return -1;
	}
	else{
		assert( false ); //should never come here
	}
}

