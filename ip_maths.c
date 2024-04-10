#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void get_broadcast_address(char *ip_addr, char mask, char* output_buffer){
   int ip[4];
   sscanf(ip_addr,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
   unsigned int subnet_mask = 0xFFFFFFFF << (32 - mask);
   unsigned int ip_address = (ip[0] << 24 | ip[1] << 16 | ip[2] << 8 | ip[3]);
   unsigned int subnet_id = ip_address & subnet_mask;
   unsigned int broadcast_address = ~subnet_mask | subnet_id;
   sprintf(output_buffer,"%d.%d.%d.%d", (broadcast_address >> 24) & 0xFF,(broadcast_address >> 16) & 0xFF,(broadcast_address >> 8) & 0xFF, broadcast_address & 0xFF);
}

unsigned int get_ip_integral_equivalent(char *ip_address){
    int ip[4];
    sscanf(ip_address,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
    unsigned int ip_integral_equivalent = pow(256,3) * ip[0] + pow(256,2) * ip[1] + pow(256,1) * ip[2] + ip[3];
    return ip_integral_equivalent; 
}

void get_abcd_format(unsigned int ip_address, char* output_buffer){
    int ip[4];
    for(int i = 3; i >= 0; i--){
        ip[i] = ip_address % 256;
        ip_address /= 256;
    }
    sprintf(output_buffer,"%d.%d.%d.%d",ip[0],ip[1],ip[2],ip[3]);
}

void net_work_id(char* ip_addr, char mask, char* output_buffer){
    unsigned int subnet_mask = 0xFFFFFFFF << (32 - mask);
    int ip[4];
    sscanf(ip_addr,"%d.%d.%d.%d",&ip[0],&ip[1],&ip[2],&ip[3]);
    unsigned int ip_address = (ip[0] << 24 | ip[1] << 16 | ip[2] << 8 | ip[3]);
    unsigned int network_id = subnet_mask & ip_address;
    sprintf(output_buffer,"%d.%d.%d.%d",(network_id >> 24) & 0xFF,(network_id >> 16) & 0xFF,(network_id >> 8) & 0xFF,network_id & 0xFF);
}

unsigned int get_subnet_cardinality(char mask){
    return pow(2,32-mask) - 2;
}

int check_ip_subnet_membership(char *network_id, char mask, char* check_ip){
    char netword_ip_address[16];
    net_work_id(check_ip,mask,netword_ip_address);
    if(strcmp(network_id,netword_ip_address) == 0)
        return 0;
    return -1;
}


int main(){
    // char output_buffer[16];
    // char *ip_addr = "192.168.2.10";
    // char mask = 20;
    // unsigned int int_ip = 2058138165;
    // get_broadcast_address(ip_addr, mask, output_buffer);
    // printf("Broadcast Address: %s",output_buffer);
    // printf("Integer equivalent for %s is %u\n",ip_addr, get_ip_integral_equivalent(ip_addr));
    // get_abcd_format(int_ip, output_buffer);
    // printf("Ip in A.B.C.D format is = %s\n",output_buffer);
    // net_work_id(ip_addr,mask,output_buffer);
    // printf("Network Id: %s\n", output_buffer);
    char* network_id = "192.168.0.0";
    char mask = 24;
    char *check_ip = "192.168.0.13";
    int result = check_ip_subnet_membership(network_id, mask,check_ip);
    printf("HAHA: %d",result);
}
