#include "common.h"

char ctrl_mes[6][50] = {"CPU","MEM","DISK","USER","SYS"};
char filepath[6][1000] = {"cpu.log","mem.log","disk.log","user.log","sys.log"},ctrl_filepath[6][50];
char buff[1024] = {0};
int n;

void InitFilePath(char* Client_IP){
  char dirname[100];
  sprintf(dirname,"./%s",Client_IP);
  printf("dirname %s \n",dirname);
  mkdir(dirname,S_IRWXG);
  char shellmod[100] = {0};
  sprintf(shellmod,"chmod -R 777 %s",Client_IP);
   printf("shell %s \n",shellmod);
  FILE  *fp=popen(shellmod,"r");
  fclose(fp);
  for (int i = 0; i < COM; i++){
    sprintf(ctrl_filepath[i],"%s/%s",dirname,filepath[i]);
    printf("%d : %s \n",i,ctrl_filepath[i]);
  }
  return ;
}

void GetClientMes(char* Client_IP){
  printf("---------------------------- \n");
  int ser_ctrl_sock = socket_connect(CTRL_PORT,Client_IP);
  send(ser_ctrl_sock,ACK1,strlen(ACK1),0);
  n = recv(ser_ctrl_sock,buff,40,0);
  buff[n] = '\0';
  printf("ACK2 : %s \n",buff);
  if (strcmp(buff,ACK2) == 0){
    int data_listen_sock = socket_create(DATA_PORT);
    int ser_data_accept_sock ;
    for (int i = 0; i < COM ; i++){
        send(ser_ctrl_sock,ctrl_mes[i],strlen(ctrl_mes[i]),0);
        printf("The mes : %s \n",ctrl_mes[i]);
        if( (ser_data_accept_sock = accept(data_listen_sock, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
        }
        FILE *fp = fopen(ctrl_filepath[i], "w");
        if (fp == NULL){
         printf("FILE Cant OPEN \n");
        }
        while (n = recv(ser_data_accept_sock, buff, 1024, 0)){
          buff[n] = '\0';
          fprintf(fp,"%s",buff);
        }
        close(ser_data_accept_sock); 
    }
    close(data_listen_sock); 
  }
  close(ser_ctrl_sock);
}

void GetWarnMes(){
   int warn_listen_sock = socket_create(WARN_PORT);
   int warn_accept_sock ;
   if( (warn_accept_sock = accept(warn_listen_sock, (struct sockaddr*)NULL, NULL)) == -1){
        printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
   }
   while (n = recv(warn_accept_sock, buff, 1024, 0)){
      buff[n] = '\0';
      printf("WARNING : %s",buff);
   }
   close(warn_accept_sock); 
   close(warn_listen_sock); 
}

void GetEventLog(char* chilt_ip)
{
    time_t t;
    struct tm * lt;
    time (&t);//获取Unix时间戳。
    lt = localtime (&t);//转为时间结构。
    FILE * fp = fopen("./event.log","w");
    if (fp == NULL)printf("FILE CANT OPEN \n");
    fprintf(fp,"%d/%d/%d %d:%d:%d ",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
    fprintf(fp, "receive %s\n",chilt_ip );
    return ;
}

int main() {
   for (int i = 0; i < 1; i++){
     InitFilePath("127.0.0.1");
     GetClientMes("127.0.0.1");
     //GetEventLog("127.0.0.1");
   } 
}
