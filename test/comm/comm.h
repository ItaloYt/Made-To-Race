#ifndef COMM_H
#define COMM_H

#include <netinet/in.h>

#include <stdbool.h>

#define PORT 32156
#define BUFFER_SIZE 1

struct socket {
    struct sockaddr_in conn;
    int handle;
    bool connected;
};
typedef struct socket socket_t;

enum packet_type {
    PACKET_TYPE_CONNECT = 0x01,
};
typedef enum packet_type packet_type_t;

#pragma pack(1)

struct connect_packet {
    packet_type_t type: 8;
};
typedef struct connect_packet connect_packet_t;

#pragma pack()

bool process_connect_packet(socket_t *sock, const struct sockaddr_in *address, const connect_packet_t *packet);
bool send_connect_packet(socket_t *sock, connect_packet_t *packet);

#endif