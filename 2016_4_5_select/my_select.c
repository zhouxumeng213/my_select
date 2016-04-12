#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/select.h>
#define _PORT_ 8080
#define _BACK_LOG_ 5
#define _MAX_FD_NUM_ 32
int array_fd[_MAX_FD_NUM_];
int startup()
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	if(sock<0){
		perror("socket");
		exit(1);
	}
	struct sockaddr_in local;
	local.sin_family=AF_INET;
	local.sin_port=htons(_PORT_);
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	socklen_t len=sizeof(local);
	if(bind(sock,(struct sockaddr*)&local,len)<0){
		perror("bind");
		exit(2);
	}
	if(listen(sock,_BACK_LOG_)<0){
		perror("listen");
		exit(3);
	}
	return sock;
}
int main()
{
	int listen_sock=startup();
	struct sockaddr_in client;
	socklen_t len=sizeof(client);
	fd_set read_set;
	int i=0;
	int max_fd=listen_sock;
	array_fd[0]=listen_sock;
	for(i=1;i<_MAX_FD_NUM_;i++){
		array_fd[i]=-1;
	}
	while(1){
		FD_ZERO(&read_set);
		for(i=0;i<_MAX_FD_NUM_;i++){
			if(array_fd[i]>0){
				FD_SET(array_fd[i],&read_set);
				if(max_fd<array_fd[i]){
					max_fd=array_fd[i];
				}
			}
		}
		struct timeval time_out={3,0};
		switch(select(max_fd+1,&read_set,NULL,NULL,&time_out)){
			case 0://timeout
				printf("timeout...\n");
				break;
			case -1://error
				perror("select");
				break;
			default:
			    {
				for(i=0;i<_MAX_FD_NUM_;i++){
					if(array_fd[i]<0){
						continue;
					}else if(array_fd[i]==listen_sock && FD_ISSET(array_fd[i],&read_set)){
						int new_sock=accept(array_fd[i],(struct sockaddr*)&client,&len);
						if(new_sock<0){
							continue;
						}
						printf("get a new connect...\n");
						for(i=0;i<_MAX_FD_NUM_;i++){
							if(array_fd[i]==-1){
								array_fd[i]=new_sock;
								break;
							}
						}
						if(i==_MAX_FD_NUM_){
							printf("array_fd is full\n");
							close(new_sock);
						}
					}else{
						for(i=1;i<_MAX_FD_NUM_;i++){
							if(array_fd[i]>0 && FD_ISSET(array_fd[i],&read_set)){
								char buf[1024];
								memset(buf,'\0',sizeof(buf)-1);
								ssize_t _size=read(array_fd[i],buf,sizeof(buf)-1);
								if(_size==0){
									printf("client close...\n");
									close(array_fd[i]);
									array_fd[i]=-1;
								}else if(_size<0){
								}else{
									printf("client: %s\n",buf);
								}
							}
						}
					}
				}
			    }
				break;
		}
	}
	close(listen_sock);
	return 0;
}
