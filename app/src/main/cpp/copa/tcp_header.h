//
// Created by ahmad.hassan on 11/16/2020.
//

#ifndef CONGESTION_TCP_HEADER_H
#define CONGESTION_TCP_HEADER_H

struct TCPHeader{
	int seq_num;
	int flow_id;
	int src_id;
	double sender_timestamp;
	double receiver_timestamp;
};

#endif //CONGESTION_TCP_HEADER_H
