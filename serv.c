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

	//INF LOOP 1
	for(;;){

		//ACCEPT
		puts("waiting to accept() clients");
		connfd = accept(listenfd, (SA*) NULL, NULL);
		if (connfd < 0)
			perror("accept error");

		printf("accepted client\n");
		//INF LOOP 2
		for(;;){
			//READ
			if (readHeaderMsg(connfd, request) < 0){	//parse through header, then read msg body, saving it to request
				fprintf(stderr, "header conversion error\n");
				break;
			}
			/*			
			//read just the header of the message
			n = 0;
			//only read until you have read all the bytes in the header
			while(n < HEADER) {
				//add the amount of bits read to n
 				n += read(connfd, c_recvLen, HEADER-n);
				
				char *ptr;
				l_recvLen = strtol(c_recvLen, &ptr, 10);
				recvLen = (int) l_recvLen;	//recvLen is the amount of bytes in the actual message
			}
			//read only recvLen bytes (which was sent to us in the header)
			n = 0;
			while(n < recvLen) {
				//add the amount of bits read to n
				n += read(connfd, recvline, recvLen-n);
				
				printf("n: [%d]\n", n);
				recvline[n] = 0;	// null terminate
				//print to console
				/ *if (fputs(recvline, stdout) == EOF)
					perror("fputs error");
				* /
				//print to char request[]
				if(sprintf(request, "%s", recvline) < 0)	//using sprintf (not snprintf) creates a potential vulnerability.
					perror("sprintf error");
				
			} */
			printf("[CLIENT] \"%s\"\n", request);	//print the message
	
			//WRITE
			char msg[] = "your message was recieved";
			
			//creates a header from msg. puts header and msg together onto buff
			makeHeader(buff, msg);
			
			printf("[SERVER] \"%s\"\n", msg);

			if(write(connfd, buff, strlen(buff)) != strlen(buff))
				perror("write error");

			//logic gate determines if we break the loop (leading to close()) or not
			if(request == "q" || 0){
				puts("break");
				break;
			}
			
		}

		//CLOSE
		puts("close");
		close(connfd);
	}
}
