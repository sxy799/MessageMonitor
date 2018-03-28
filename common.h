#include <time.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/file.h>
#include <netinet/in.h>
#include "netdb.h"
#include "stdio.h"
#include "stdlib.h"
#include "sys/socket.h"
#include "unistd.h"
#include "arpa/inet.h"
#include "string.h"
#include "memory.h"
#include "signal.h"
#include "time.h"
#include <string>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>


#define DBUG(format, ...) fprintf(stdout, format, __VA_ARGS__)

#define ACK1 "ACK1"
#define ACK2 "ACK2"

#define CPU  "CPU"
#define MEM "MEM"
#define DISK "DISK"
#define USER "USER"
#define SYS "SYS"

#define COM 5


int socket_create(int port);
int socket_connect(int port,char *host);
int get_conf_value(char* pathname, char* key_name, char *value);
int GetConfig();

int socket_connect(int port,char *host){
    int sockfd;
    struct sockaddr_in dest_addr;
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0){
        perror("socket error");
        return 1;
    }

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);
    dest_addr.sin_addr.s_addr = inet_addr(host);

    fflush(stdout);
    if (connect(sockfd,(struct sockaddr *)&dest_addr,sizeof(dest_addr)) < 0){
        perror("connect error ");
        return -1;
    }
    return sockfd;
}

int socket_create(int port){
    int sockfd;
    int yes = 1;
    struct sockaddr_in sock_addr;

    //创建套接字
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket() error\n");
        return -1;
    }

    //设置本地套接字地址
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port); //转化为网络字节序
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY); //0.0.0.0

    //设置本地套接字
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        close(sockfd);
        perror("setsockopt() error\n");
        return -1;
    }

    //绑定本地套接字到套接字
    if (bind(sockfd, (struct sockaddr *) &sock_addr, sizeof(sock_addr)) < 0) {
        close(sockfd);
        perror("bind() error\n");
        return -1;
    }

    //将套接字设置为监听状态
    if (listen(sockfd, 20) < 0) {
        close(sockfd);
        perror("listen() error");
        return -1;
    }
     
     /*
    struct sockaddr_in client_add;
    socklen_t l = sizeof(sock_addr);
    int listen_soc = accept(sockfd,(struct sockaddr*) &client_add, &l);
    if (listen_soc < 0){
        printf("accept ERROR\n");
    }else {
        printf("accept OK\n");
    }*/
    return sockfd;
}


int socket_accept(int listen_sock){
    struct sockaddr_in client_add;
    int accept_sock;
    //socklen_t l = sizeof(sock_addr);
    int accept_soc = accept(listen_sock,(struct sockaddr*)NULL, NULL);
    if (accept_soc < 0){
        printf("accept ERROR\n");
    }else {
        printf("accept OK\n");
    }
    return accept_sock;
}

int get_conf_value(char* pathname, char* key_name, char *value) {
 
    FILE *fp = NULL;
    char *line = NULL, *substr = NULL;
    size_t len = 0, tmplen = 0;
    ssize_t read;
    //memset(value, 0, sizeof(char)*MAX_SIZE);
 
    if ( key_name == NULL || value == NULL) {
       // DBG("paramer is invaild!n");
        exit(-1);
    }
 
    fp = fopen(pathname,"r");
    if (fp == NULL) {
      //  DBG("Open config file error!\n");
        exit(-1);
    }
 
    while (( read = getline(&line, &len,fp)) != -1) {
        substr = strstr(line, key_name);
        if (substr == NULL) 
            continue;
        else {
            tmplen = strlen(key_name);
            if (line[tmplen] == '=') {
                strncpy(value, &line[tmplen + 1], (int)read - tmplen + 1);
                tmplen = strlen(value);
                *(value + tmplen - 1) = ' ';
                break;
            }
            else {
              // DBG("Maybe there is something wrong with config file!n");
                continue;
            }
        }
    }
 
    if (substr == NULL) {
       // DBG("%s not found in config file!n", key_name);
        fclose(fp);
        exit(-1);
    }
 
    //DBG("%s=%sn", key_name, value);
    free(line);
    fclose(fp);
    return 0;
}
int NUM;
int CTRL_PORT;
int DATA_PORT;
int WARN_PORT;
char str[50];
char MASTER_IP[100];
char Client_Ip[50][50];
int a = GetConfig();

int GetConfig(){
   printf(" ------------------------------------------\n");
   get_conf_value("master.conf","num",str);
   NUM = atoi(str);
   printf("NUM : %d \n",NUM);
   get_conf_value("master.conf","MASTER_IP",MASTER_IP);
   printf("MASTER_IP: %s \n",MASTER_IP);
   get_conf_value("master.conf","ctrl_port",str);
   CTRL_PORT = atoi(str);
   printf("CTRL_PORT: %d \n",CTRL_PORT);
   get_conf_value("master.conf","data_port",str);
   DATA_PORT = atoi(str);
   printf("DATA_PORT: %d \n",DATA_PORT);
   get_conf_value("master.conf","warn_port",str);
   WARN_PORT = atoi(str);
   printf("WARN_PORT: %d \n",WARN_PORT);
   for(int i = 0; i < NUM; i++){
       char key[50];
       sprintf(key,"CLIENTIP%i",i);
       //printf("key :%s \n",key);
       get_conf_value("master.conf",key,Client_Ip[i]);
       printf("Client_Ip[%d] :%s \n",i,Client_Ip[i]);
   }
   printf(" ------------------------------------------\n");
   return 1;
}
