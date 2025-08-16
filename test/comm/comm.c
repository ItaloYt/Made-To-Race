#include <netinet/in.h>

#include <stdbool.h>

#include "comm.h"

bool process_connect_packet(socket_t *sock, const struct sockaddr_in *address, const connect_packet_t *packet) {
    if (sock->connected) return false;

    sock->conn = *address;
    sock->connected = true;

    return false;
}

bool send_connect_packet(socket_t *sock, connect_packet_t *packet) {
    packet->type = PACKET_TYPE_CONNECT;

    struct sockaddr_in address = {0};

    if (sock->connected) address = sock->conn;
    else {
        address.sin_family = AF_INET;
        address.sin_port = htons(PORT);
        address.sin_addr.s_addr = INADDR_BROADCAST;
    }

    if (sendto(sock->handle, packet, sizeof(connect_packet_t), 0, (struct sockaddr *) &address, sizeof(address)) == -1)
        return true;

    return false;
}
