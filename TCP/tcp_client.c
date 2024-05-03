#include <stdio.h>
#include <stdlib.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include "common.h"
#include <sys/select.h> // Include the sys/select.h header for select()
#include <arpa/inet.h> // Include the arpa/inet.h header for inet_ntoa()
#include <unistd.h> // Include the unistd.h header for close()

#define DEST_PORT 2000
#define SERVER_IP_ADDRESS "127.0.0.1"

test_struct_t client_data;
result_struct_t result;

void setup_tcp_connection() {
    /* Step 1: Initialization */
    int sockfd = 0;
    int sent_recv_bytes = 0;
    socklen_t addr_len = 0;

    /* Step 2: Socket and server address */
    struct sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(DEST_PORT);
    dest.sin_addr.s_addr = inet_addr(SERVER_IP_ADDRESS);

    /* Step 3: Create a TCP socket */
    if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    /* Step 4: Connect to the server */
    if (connect(sockfd, (struct sockaddr *)&dest, sizeof(struct sockaddr)) == -1) {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }

PROMPT_USER:
    printf("Enter a: ");
    scanf("%u", &client_data.a);
    printf("Enter b: ");
    scanf("%u", &client_data.b);

    /* Step 5: Send data to the server */
    sent_recv_bytes = send(sockfd, &client_data, sizeof(test_struct_t), 0);
    if (sent_recv_bytes == -1) {
        perror("send failed");
        exit(EXIT_FAILURE);
    }
    printf("No of bytes sent = %d\n", sent_recv_bytes);

    /* Step 6: Receive data from the server */
    sent_recv_bytes = recv(sockfd, (char *)&result, sizeof(result_struct_t), 0);
    if (sent_recv_bytes == -1) {
        perror("recv failed");
        exit(EXIT_FAILURE);
    }
    printf("No of bytes received = %d\n", sent_recv_bytes);
    printf("Result received = %u\n", result.c);

    goto PROMPT_USER;
}




int main(int argc, char **argv){
    setup_tcp_connection();
    return 0;
}