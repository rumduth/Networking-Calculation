#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/_endian.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <errno.h>
#include <sys/select.h> // Include the sys/select.h header for select()
#include <arpa/inet.h> // Include the arpa/inet.h header for inet_ntoa()
#include <unistd.h> // I
#include <ctype.h>


#define SERVER_PORT 2001
char data_buffer[1024];

typedef struct student_{
    char name[32];
    unsigned int roll_no;
    char hobby[32];
    char dept[32];
}student_t;

student_t student[5] = {
    {"Abhishek", 10305042, "Programming", "CSE"},
    {"Nitin", 10305048, "Programming", "CSE"},
    {"Avinash", 10305041, "Cricket", "ECE"},
    {"Jack", 10305032, "Udemy Teaching", "Mechanical"},
    {"Cris", 10305030, "Programming", "Electrical"}
};

void string_space_trim(char *string){
    if(!string)
        return;
    char* ptr = string;
    int len = strlen(ptr);
    if(!len){
        return;
    }
    if(!isspace(ptr[0]) && !isspace(ptr[len-1])){
        return;
    }
    while(len - 1 > 0 && isspace(ptr[len-1])){
        ptr[--len] = 0;
    }
    while(*ptr && isspace(*ptr)){
        ++ptr;
        --len;
    }
    memmove(string,ptr,len+1);

}

static char* process_POST_request(char* URL, unsigned int* response_len){
    return NULL;
}

static char * process_GET_request(char * URL, unsigned int* response_len){
    printf("%s(%u) called with URL = %s\n",__FUNCTION__,__LINE__,URL);
    char delimiter [2] = {'?','\0'};
    string_space_trim(URL);
    char *token[5] = {0};
    token[0] = strtok(URL,delimiter);
    token[1] = strtok(0, delimiter);
    delimiter[0] = '&';
    token[2] = strtok(token[1], delimiter); //Department
    token[3] = strtok(0, delimiter);//Roll Number

    printf("token[0] = %s, token[1] = %s, token[2] = %s, token[3] = %s \n",token[0],token[1],token[2],token[3]);
    delimiter[0] = '=';
    char* roll_no_str = strtok(token[3],delimiter);
    char *roll_no_value = strtok(0, delimiter);
    printf("Roll no value = %s\n",roll_no_value);

    unsigned int roll_no = atoi(roll_no_value), i = 0;
    for(; i < 5; i++){
        if(student[i].roll_no != roll_no){
            continue;
        }
        break;
    }
    if(i == 5){
        return NULL;
    }

    char *response = calloc(1,1024);
    strcpy(response,
        "<html>"
        "<head>"
            "<title>HTML Response</title>"
            "<style>"
            "table, th, td {"
                "border: 1px solid black;}"
             "</style>"
        "</head>"
        "<body>"
        "<table>"
        "<tr>"
        "<td>");
    strcat(response, student[i].name);
    strcat(response, "</td></tr>");
    strcat(response, 
            "</table>"
            "</body>"
            "</html>");
    

    unsigned int content_len_str = strlen(response);

    char *header  = calloc(1, 248 + content_len_str);
    strcpy(header, "HTTP/1.1 200 OK\n");      
    strcat(header, "Server: My Personal HTTP Server\n");
    strcat(header,"Content-length: 2048\n");
    strcat(header, "Connection: close\n");
    strcat(header, "Content-Type: text/html; charset=UTF-8\n");
    strcat(header, "\n");


    strcat(header,response);
    content_len_str = strlen(header);
    *response_len = content_len_str;
    free(response);
    return header;


  
}


void setup_tcp_server_communication(){
    int master_socket_tcp_fd = 0,
        sent_recv_bytes = 0,
        opt = 1;
    socklen_t addr_len = 0;
    int comm_socket_fd = 0;
    fd_set readfds;

    struct sockaddr_in server_addr, client_addr;

    if((master_socket_tcp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
        printf("socket creation failed\n");
        return;
    }

    if(setsockopt(master_socket_tcp_fd, SOL_SOCKET, SO_REUSEADDR,(char*)&opt, sizeof(opt)) < 0){
        printf("TCP socket creation failed for multiple connections\n");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    addr_len = sizeof(struct sockaddr);


    if(bind(master_socket_tcp_fd,(struct sockaddr*)&server_addr,sizeof(struct sockaddr)) == -1){
        printf("socket bind failed\n");
        return;
    }

    if(listen(master_socket_tcp_fd, 5) < 0){
        printf("listen failed\n");
        return;
    }
    while(1){
        FD_ZERO(&readfds);
        FD_SET(master_socket_tcp_fd,&readfds);
        printf("Blocked on select System Call...\n");

        select(master_socket_tcp_fd + 1,&readfds,NULL, NULL, NULL);
        if(FD_ISSET(master_socket_tcp_fd,&readfds)){
            printf("New connection received recvd, accept the connection. Client and Server completes TCP-3 way handshae at this point\n");
            comm_socket_fd = accept(master_socket_tcp_fd,(struct sockaddr*)& client_addr, &addr_len);
            if(comm_socket_fd < 0){
                printf("accept error: errno = %d\n",errno);
                exit(0);
            }
            printf("Connection accepted from client: %s:%u\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
            while(1){
                printf("Server ready to service client msgs.\n");
                memset(data_buffer,0, sizeof(data_buffer));
                sent_recv_bytes = recv(comm_socket_fd,(char*)data_buffer,sizeof(data_buffer),0);
                printf("Server received %d bytes from client %s:%u\n", sent_recv_bytes, inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
                if(sent_recv_bytes == 0){
                    close(comm_socket_fd);
                    break;
                }

                printf("Msg received: %s\n",data_buffer);
                char *request_line = NULL;
                char del[2]= "\n",
                    *method = NULL,
                    *URL = NULL;
                request_line = strtok(data_buffer,del);
                del[0] = ' ';
                method = strtok(request_line,del);
                URL = strtok(NULL,del);
                printf("Method = %s\n",method);
                printf("URL = %s\n",URL);
                char* response = NULL;
                unsigned int response_length = 0;
                if(strncmp(method, "GET", strlen("GET")) == 0){
                    response = process_GET_request(URL,&response_length);
                }else if(strncmp(method, "POST",strlen("POST")) == 0){
                    response = process_POST_request(URL,&response_length);
                }else{
                    printf("Unsuppored URL method request\n");
                    close(comm_socket_fd);
                    break;
                }

                if(response){
                    printf("response to be sent to client = \n%s",response);
                    sent_recv_bytes = send(comm_socket_fd,response,response_length, 0);
                    free(response);
                    printf("Server sent %d bytes in reply to client\n",sent_recv_bytes);
                    close(comm_socket_fd);
                }

            }
        }
    }




}

int main(int argc, char** argv){
    setup_tcp_server_communication();
    return 0;
}