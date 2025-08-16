#include <linux/limits.h>
#include <string.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>
#include <arpa/inet.h>
#include <assert.h>

#include "comm.h"

socket_t server;

bool update();

int main() {
    server.handle = socket(AF_INET, SOCK_DGRAM, 0);

    if (server.handle == -1) {
        (void) fprintf(stderr, "socket: %s\n", strerror(errno));

        return 1;
    }

    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server.handle, (struct sockaddr *) &address, sizeof(address)) == -1) {
        (void) fprintf(stderr, "bind: %s\n", strerror(errno));

        close(server.handle);
        
        return 1;
    }

    while (true) {
        update();

        (void) usleep(100);
    }

    close(server.handle);

    return 1;
}

bool update() {
    static char buffer[BUFFER_SIZE];
    static struct sockaddr_in address = {0};

    int status = recvfrom(server.handle, buffer, BUFFER_SIZE, 0, (struct sockaddr *) &address, &(socklen_t) { sizeof(address) });

    if (status == -1 && errno != EAGAIN) {
        (void) fprintf(stderr, "Failed to receive packet: %s", strerror(errno));

        return true;
    }

    if (status > 0) {
        switch (buffer[0]) {
            case PACKET_TYPE_CONNECT:
                if (process_connect_packet(&server, &address, (const connect_packet_t *) buffer)) {
                    (void) fprintf(stderr, "Failed to process connect packet");

                    return true;
                }

                if (server.connected && send_connect_packet(&server, (connect_packet_t *) buffer)) {
                    (void) fprintf(stderr, "Failed to send connect packet");
                    
                    return true;
                }

                break;

            default:
                (void) fprintf(stderr, "Unknown packet type: %x", buffer[0]);

                return true;
        }
    }

    if (server.connected) {
        (void) printf("Connected\n");
    }

    return false;
}
