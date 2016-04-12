#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<unistd.h>
#define _PORT_ 8080
int main()
{
	int read_fd=0;
	int write_fd=1;
	fd_set read_set;
	fd_set write_set;
	int max_fd=0;
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("socket");
		exit(1);
	}
	struct sockaddr_in remote;
	remote.sin_family=AF_INET;
	remote.sin_port=htons(_PORT_);
	remote.sin_addr.s_addr=inet_addr("192.168.0.146");
	if(connect(sock,(struct sockaddr*)&remote,sizeof(remote))<0){
		perror("connect");
		exit(2);
	}
	if(sock>read_fd){
		max_fd=sock;
	}else{
		max_fd=read_fd;
	}
	while(1){
		FD_ZERO(&read_set);
		FD_ZERO(&write_set);
		FD_SET(read_fd,&read_set);
		FD_SET(sock,&write_set);
		switch(select(max_fd+1,&read_set,&write_set,NULL,NULL)){
			case 0:
				printf("timeout...\n");
				break;
			case -1:
				perror("select");
				break;
			default:
				{
					if(FD_ISSET(read_fd,&read_set)){
						char buf[1024];
						memset(buf,'\0',sizeof(buf));
						ssize_t _size=read(read_fd,buf,sizeof(buf)-1);
						if(_size>0){
							buf[_size]='\0';
							printf("echo: %s\n",buf);
						}
						if(FD_ISSET(sock,&write_set)){
							send(sock,buf,strlen(buf),0);
						}
					}
				}
				break;
		}
	}
	return 0;
}
