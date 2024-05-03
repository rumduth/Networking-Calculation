#include <stdio.h>
#include <stdlib.h>
#include <sys/_types/_socklen_t.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "common.h"

#define MAX_CLIENT_SUPPORTED 32
#define SERVER_PORT 2000

test_struct_t test_struct;
result_struct_t res_struct;
char data_buffer[1024];
int monitor_fd_set[32];


static void initialize_monitor_fd_set(){
  for(int i = 0; i < MAX_CLIENT_SUPPORTED; i++){
    monitor_fd_set[i] = -1;
  }
}
static void add_to_monitor_fd_set(int skt_fd){
    for(int i = 0; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitor_fd_set[i] != -1){
            continue;
        }else{
            monitor_fd_set[i] = skt_fd;
            break;
        }
    }
}
static void re_init_readfds(fd_set *fd_set_ptr){
    FD_ZERO(fd_set_ptr);
    for(int i = 0; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitor_fd_set[i] != -1){
            FD_SET(monitor_fd_set[i],fd_set_ptr);
        }
    }
}

static int get_max_fd() {
    int max_fd = -1;
    for (int i = 0; i < MAX_CLIENT_SUPPORTED; i++) {
        if (monitor_fd_set[i] > max_fd) {
            max_fd = monitor_fd_set[i];
        }
    }
    return max_fd;
}
static void remove_from_monitored_fd_set(int fd){
    for(int i = 0; i < MAX_CLIENT_SUPPORTED; i++){
        if(monitor_fd_set[i] == fd){
            monitor_fd_set[i] = -1;
            break;
        }
    }
}

void setup_tcp_server_communication(){
    int master_sock_tcp_fd = 0,
        sent_recv_bytes = 0,
        opt = 1;
    socklen_t addr_len = 0;

    int comm_socket_fd = 0;
    fd_set readfds;
    struct sockaddr_in server_addr, client_addr;

    initialize_monitor_fd_set();
    if((master_sock_tcp_fd = socket(AF_INET,SOCK_STREAM, IPPROTO_TCP)) == -1){
        printf("Socket Creation Failed\n");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    addr_len = sizeof(struct sockaddr);
    if(bind(master_sock_tcp_fd,(struct sockaddr*)&server_addr, sizeof(struct sockaddr)) == -1){
        printf("Socket Bind Failed\n");
        return;
    }
    if(listen(master_sock_tcp_fd,MAX_CLIENT_SUPPORTED) < 0){
        printf("listen failed\n");
        return;
    }
    add_to_monitor_fd_set(master_sock_tcp_fd);
    while(1){
        re_init_readfds(&readfds);
        printf("Blocked on select System Call...\n");
        select(get_max_fd() + 1, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(master_sock_tcp_fd,&readfds)){ //->Data arrive to master socket
            printf("New connection received, accept the connection. Client and Server completes TCP-3 way handshake at this point");
            comm_socket_fd = accept(master_sock_tcp_fd,(struct sockaddr*)&client_addr, &addr_len);
            if(comm_socket_fd < 0){
                printf("accept error: errno = %d\n",errno);
                exit(0);
            }
            add_to_monitor_fd_set(comm_socket_fd);
            printf("Connection accepted from client: %s:%u\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
        }else{ //-> Data arrive on some other FD
            int i = 0, comm_socket_fd = -1;
            for(; i < MAX_CLIENT_SUPPORTED; i++){
                if(FD_ISSET(monitor_fd_set[i],&readfds)){
                    comm_socket_fd = monitor_fd_set[i];
                    memset(data_buffer, 0, sizeof(data_buffer));
                    sent_recv_bytes = recv(comm_socket_fd, (char*)data_buffer, sizeof(data_buffer), 0);
                    printf("Server recvd %d bytes from client %s: %u\n",sent_recv_bytes, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                    if(sent_recv_bytes == 0){
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd);
                        break;
                    }
                    test_struct_t *client_data = (test_struct_t *)data_buffer;
                    if(client_data->a == 0 && client_data->b == 0){
                        close(comm_socket_fd);
                        remove_from_monitored_fd_set(comm_socket_fd);
                        printf("Server closes connection with client : %s:%u\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                        break;
                    }
                    result_struct_t result;
                    result.c = client_data->a + client_data->b;
                    sent_recv_bytes = send(comm_socket_fd, (char*)&result, sizeof(result_struct_t), 0);
                    printf("Server sebt %d bytes in reply to client\n",sent_recv_bytes);
                }
            }
        }
    }
}


int main(int argc, char **argv) {
    setup_tcp_server_communication();
    return 0;
}
