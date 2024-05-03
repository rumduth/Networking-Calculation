#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "common.h"

#define SERVER_PORT 2000



void setup_tcp_server_communication() {
    int master_sock_tcp_fd = 0;
    int sent_recv_bytes = 0;
    socklen_t addr_len;
    int opt = 1;

    int comm_socket_fd = 0;
    fd_set readfds;

    struct sockaddr_in server_addr, client_addr;

    if ((master_sock_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    if (bind(master_sock_tcp_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("socket bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(master_sock_tcp_fd, 5) < 0) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    addr_len = sizeof(struct sockaddr);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(master_sock_tcp_fd, &readfds);
        printf("Blocked on select System call...\n");

        select(master_sock_tcp_fd + 1, &readfds, NULL, NULL, NULL);
        
        if (FD_ISSET(master_sock_tcp_fd, &readfds)) {
            printf("New connection received, accepting the connection.\n");
            
            comm_socket_fd = accept(master_sock_tcp_fd, (struct sockaddr *)&client_addr, &addr_len);
            if (comm_socket_fd < 0) {
                perror("accept error");
                continue;
            }
            
            printf("Connection accepted from client: %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

            while (1) {
                printf("Server ready to service client messages.\n");
                test_struct_t client_data;
                result_struct_t result;

                memset(&client_data, 0, sizeof(client_data));
                memset(&result, 0, sizeof(result));

                sent_recv_bytes = recv(comm_socket_fd, (char *)&client_data, sizeof(client_data), 0);
                if (sent_recv_bytes == 0) {
                    close(comm_socket_fd);
                    printf("Connection closed by client.\n");
                    break;
                } else if (sent_recv_bytes < 0) {
                    perror("recv error");
                    close(comm_socket_fd);
                    break;
                }

                if (client_data.a == 0 && client_data.b == 0) {
                    close(comm_socket_fd);
                    printf("Server closing connection with client: %s:%u\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    break;
                }

                result.c = client_data.a + client_data.b;
                sent_recv_bytes = send(comm_socket_fd, (char *)&result, sizeof(result), 0);
                if (sent_recv_bytes < 0) {
                    perror("send error");
                    close(comm_socket_fd);
                    break;
                }

                printf("Server sent %d bytes in reply to client.\n", sent_recv_bytes);
            }
        }
    }
}

int main(int argc, char **argv) {
    setup_tcp_server_communication();
    return 0;
}
