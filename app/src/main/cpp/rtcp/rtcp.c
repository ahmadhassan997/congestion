//
// Created by ahmad.hassan on 11/29/2020.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <jni.h>
#include <android/log.h>
#include "tcp.h"


// first it should connect to the server and then receive packets
void run(const char* src_ip, const char* tgt_ip, const char* src_p, const char* tgt_p, const char* f_name, double dur) {
    __android_log_print(ANDROID_LOG_DEBUG, "rtcp", "starting raw socket");
    double timeToRun = dur;
    FILE *fptr;
    fptr = fopen(f_name, "w");
    // exiting program
    if (fptr == NULL) {
        printf("Error!");
        exit(1);
    };

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock == -1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "rtcp", "socket creation failed");
        return;
    }

    // destination IP address configuration
    struct sockaddr_in daddr;
    daddr.sin_family = AF_INET;
    daddr.sin_port = htons(atoi(tgt_p));
    if (inet_pton(AF_INET, tgt_ip, &daddr.sin_addr) != 1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "rtcp", "destination IP configuration failed");
        return;
    }

    // source IP address configuration
    struct sockaddr_in saddr;
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(atoi(src_p));
    if (inet_pton(AF_INET, src_ip, &saddr.sin_addr) != 1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "rtcp", "source IP configuration failed");
        return;
    }

    printf("client listening on %u[%s], dest port is %u[%s]\n", saddr.sin_port, src_p, daddr.sin_port, tgt_p);

    // tell the kernel that headers are included in the packet
    int one = 1;
    const int *val = &one;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) == -1)
    {
        __android_log_print(ANDROID_LOG_ERROR, "rtcp", "setsockopt (IP_HDRINCL, 1)s failed");
        return;
    }

    // send SYN
    char* packet;
    int packet_len;
    create_syn_packet(&saddr, &daddr, &packet, &packet_len);

    int sent;
    if ((sent = sendto(sock, packet, packet_len, 0, (struct sockaddr*)&daddr, sizeof(struct sockaddr))) == -1)
    {
        printf("sendto() failed\n");
    }
    else
    {
        printf("successfully sent %d bytes SYN!\n", sent);
    }

    // receive SYN-ACK
    char recvbuf[DATAGRAM_LEN];
    int received = receive_from(sock, recvbuf, sizeof(recvbuf), &saddr);
    if (received <= 0)
    {
        printf("receive_from() failed\n");
    }
    else
    {
        printf("successfully received %d bytes SYN-ACK!\n", received);
    }

    // read sequence number to acknowledge in next packet
    uint32_t seq_num, ack_num;
    read_seq_and_ack(recvbuf, &seq_num, &ack_num);
    int new_seq_num = seq_num + 1;

    // send ACK
    // previous seq number is used as ack number and vice vera
    create_ack_packet(&saddr, &daddr, ack_num, new_seq_num, &packet, &packet_len);
    if ((sent = sendto(sock, packet, packet_len, 0, (struct sockaddr*)&daddr, sizeof(struct sockaddr))) == -1)
    {
        printf("sendto() failed\n");
    }
    else
    {
        printf("successfully sent %d bytes ACK!\n", sent);
    }

    // receive data

    struct timeval startTime;
    struct timeval currentTime;
    double relativeTime=0;
    gettimeofday(&startTime,NULL);
    while (relativeTime <= timeToRun)
    {
        received = receive_from(sock, recvbuf, sizeof(recvbuf), &saddr);
        printf("successfully received %d bytes!\n", received);
        read_seq_and_ack(recvbuf, &seq_num, &ack_num);
        new_seq_num = seq_num + 1;
        create_ack_packet(&saddr, &daddr, ack_num, new_seq_num, &packet, &packet_len);
        if ((sent = sendto(sock, packet, packet_len, 0, (struct sockaddr*)&daddr, sizeof(struct sockaddr))) == -1)
        {
            printf("send failed\n");
        }
        else
        {
            printf("successfully sent %d bytes ACK!\n", sent);
        }
        gettimeofday(&currentTime, NULL);
        relativeTime = (currentTime.tv_sec-startTime.tv_sec)+(currentTime.tv_usec-startTime.tv_usec)/1000000.0;
    }
    close(sock);
}

