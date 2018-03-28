#include "common.h"

char buff[1024];
char mes[6][50] = {"CPU","MEM","DISK","USER","SYS"};
char filepath[6][50] = {"./log/cpu.log","./log/mem.log","./log/disk.log","./log/user.log","./log/sys.log"};
int n;

void SentClientMes(){
  memset(buff,0,sizeof(buff));
  int cli_ctrl_listen_sock = socket_create(CTRL_PORT),cli_ctrl_accept_sock;
  if( (cli_ctrl_accept_sock = accept(cli_ctrl_listen_sock, (struct sockaddr*)NULL, NULL)) == -1){
    printf("accept socket error: %s(errno: %d)",strerror(errno),errno);
  }
  int n = recv(cli_ctrl_accept_sock, buff, 1024, 0);
  buff[n] = '\0';
  printf("ACK1 : %s\n", buff);
  if (strcmp(buff,ACK1) == 0){
      printf("ACK 1\n");
      send(cli_ctrl_accept_sock,ACK2,strlen(ACK2),0);
      for (int i = 0; i < 5; i++){
        n = recv(cli_ctrl_accept_sock,buff,40,0);
        buff[n] = '\0';
        printf("Sig : %s \n",buff);
        if (strcmp(buff,mes[i]) == 0){
          int data_sock = socket_connect(DATA_PORT,MASTER_IP);
          FILE *fp = fopen(filepath[i], "r");
          flock(fp->_fileno,LOCK_EX);
          while (fgets(buff,1024,fp) != NULL){
           // printf("send : %s \n",buff);
            send(data_sock,buff,strlen(buff),0);
            memset(buff,0,sizeof(buff));
          }
          printf("Cilent send %s over !\n",filepath[i]);
          fclose(fp);
          fp = fopen(filepath[i], "w");
          fclose(fp);
          close(data_sock);
        } 
    }
  }
  close(cli_ctrl_listen_sock);
  close(cli_ctrl_accept_sock);
}
void SentWarnMes(){
  memset(buff,0,sizeof(buff));
  int warn_sock = socket_connect(WARN_PORT,MASTER_IP);
  FILE *fp = fopen("./log/warn.log", "r");
  flock(fp->_fileno,LOCK_EX);
  while (fgets(buff,1024,fp) != NULL){
   // printf("send : %s \n",buff);
    send(warn_sock,buff,strlen(buff),0);
    memset(buff,0,sizeof(buff));
  }
  printf("Cilent send warn over !\n");
  fclose(fp);
  fp = fopen("./log/warn.log", "w");
  fclose(fp);
  close(warn_sock);
}

void GetHealthMes(){
  FILE* fp;
  char str[105];
  char buffer[30000];
  char BASH[6][20] = {"bash getcpu.sh","bash getmem.sh  40","bash getdisk.sh","bash getuser.sh","bash getsys.sh"};
  char FILEPATH[6][20] = {"./log/cpu.log","./log/mem.log","./log/disk.log","./log/user.log","./log/sys.log"};

  for (int com = 0; com < COM; com++){
     memset(buffer,0,sizeof(buffer));
    for (int i = 0; i < 12; i++){
      fp = popen(BASH[com],"r");
      fgets(str,1024,fp);
     // printf("str %d: %s \n",strlen(str),str);
      strncat(buffer,str,strlen(str));
      //printf("buff : %s\n",buffer);
      memset(str,0,sizeof(str));
      fclose(fp);
      sleep(1);
    }

    fp = fopen(FILEPATH[com],"a+");
    if (fp == NULL) { 
      printf("ERROR \n");
      exit(1);
    }
    fprintf(fp,"%s",buffer);
    fclose(fp);
   // printf("buff : %s \n",buff );
 }
  return ;
}

void GetEventLog()
{
    time_t t;
    struct tm * lt;
    time (&t);//获取Unix时间戳。
    lt = localtime (&t);//转为时间结构。
    FILE * fp = fopen("./log/event.log","w");
    if (fp == NULL)printf("FILE CANT OPEN \n");
    fprintf(fp,"%d/%d/%d %d:%d:%d ",lt->tm_year+1900, lt->tm_mon, lt->tm_mday, lt->tm_hour, lt->tm_min, lt->tm_sec);//输出结果
    fprintf(fp, "syn to %s\n",MASTER_IP );
    return ;
}

int main(){
    int pid[5];
    int a = 0;
    for(int i = 0; i < 3; i++){
        pid[i] = fork();
        if (pid[i] > 0) exit(1);
        if(pid[i] == 0){
            if (i == 0 ){
              printf("1 %d\n",a+1);
                while (1){
                   GetHealthMes();
                }
             }
            else if (i == 1) {
              printf("2 %d \n",a+2);
              SentClientMes();
              GetEventLog();
            }
            else{
              printf("warning %d \n",a+3);
            }
        }
   }
  return 0;
}