#include <stdio.h>
#include <stdlib.h>		//strtol()
#include <unistd.h>		//read()
#include <sys/types.h>
#include <sys/socket.h>	//socket()
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>	//threads

#include "h/utility.h"

int main(int argc, char *argv[]){
	int					listenfd, connfd, n, recvLen;
	struct sockaddr_in	servaddr;
	char				recvline[MAXLINE+1], c_recvLen[HEADER], request[MAXLINE+1], buff[MAXLINE];
	long int			l_recvLen;
	
	//CREATE SOCKET
	listenfd = socket(AF_INET, SOCK_STREAM, 0);

	if (listenfd < 0)
		perror("socket error");


	//SETSOCKOPT
	if(setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0)
		perror("setsockopt error");

	//SERVADDR VALUES
	bzero(&servaddr, sizeof(servaddr));				//clear values
	servaddr.sin_family      = AF_INET;				//af_inet is the Internet
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	//convert ip to long
	servaddr.sin_port        = htons(PORT);			//port 13 for Daytime Server

	//BIND
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		perror("bind error");

	//LISTEN
	if(listen(listenfd, LISTENQ) < 0)
		perror("listen error");

	//Loop to ACCEPT clients
	for(;;){

		//ACCEPT
		puts("waiting to accept() clients");
		connfd = accept(listenfd, (SA*) NULL, NULL);
		if (connfd < 0)
			perror("accept error");

		printf("*****\naccepted client\n");
		//Loop to handle individual clients
		for(;;){
			//READ
			n = 0;
		
			//empty the buff
			memset(buff, 0, sizeof(buff));
	
			//read bytes until we find '\n' or if we hit 50 total bytes.
			do{
				//buff+n so we don't overwrite previous reads.
				n += Read(connfd, buff+n, 1);
			}while(buff[n-1] != '\n' && n < 50);

			//print what we got
			printf("[CLIENT] \"%s\"\n", buff);
			
			//WRITE
			char msg[] = "your message was recieved";
		
			//empty the buff
			memset(buff, 0, sizeof(buff));
	
			//append \r\n to message
			sprintf(buff, "%s\n", msg);
			
			printf("[SERVER] > {%s}\n", buff);

			if(write(connfd, buff, strlen(buff)) != strlen(buff))
				perror("write error");

			//logic gate determines if we break the loop (leading to close()) or not
			if(request == "q" || 1){
				puts("break");
				break;
			}
			
		}

		//CLOSE
		puts("close");
		close(connfd);
	}
}
