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
	char 				quit[] = "\r\n\r\n";
	
	int					listenfd, connfd, n, recvLen, request;
	struct sockaddr_in	servaddr;
	char				recvline[MAXLINE+1], c_recvLen[HEADER], buff[MAXLINE], msg[MAXLINE-strlen(quit)];
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

			
			//read bytes until we find "\r\n\r\n" or if we hit MAXLINE total bytes.
			while(n <= MAXLINE){
				//read from fd
 				int err = Read(connfd, buff+n, 1);
				n += err;
				if(err < 0){
					break;
				}
			
				//checks if you have the quit sequence at the end of ur buffer
				if (n >= 4 && substring(buff, quit, n-1)){
					break;
				}
	
			}
		
			//print what we got
			printf("[CLIENT]: \n\n%s[SERVER DESIGNATED END]\n\n", buff);
	
			//PROCESS TRANSMISSION
			char requestOptions[9][8] = {"GET", "HEAD", "POST", "PUT", "DELETE", "CONNECT", "OPTIONS", "TRACE", "PATCH"};
			request = -1;
			for (int i = 0; i < 9; ++i){
				if (substring(buff, requestOptions[i], strlen(requestOptions[i])-1 )){
					request = i;
					break;
				}
			}
			switch(request){
				case 0:
					//saves index.html to msg, we're gonna send it to the client
					fileToString("files/index.html", msg);
					break;
			}
			
			

			
			//WRITE
		
			//empty the buff
			memset(buff, 0, sizeof(buff));
	
			//append \r\n to message
			sprintf(buff, "%s\r\n", msg);
			
			printf("[SERVER] > {%s}\n", buff);

			if(write(connfd, buff, strlen(buff)) != strlen(buff))
				perror("write error");

			//logic gate determines if we break the loop (leading to close()) or not
			if(1){
				puts("break");
				break;
			}
			
		}

		//CLOSE
		puts("close");
		close(connfd);
	}
}
