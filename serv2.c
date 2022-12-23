#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SA 		struct sockaddr
#define PORT 	13
#define MAXLINE	4096
#define LISTENQ	1024

int main(int argc, char *argv[]){
	int					listenfd, connfd, n;
	struct sockaddr_in	servaddr;
	char				recvline[MAXLINE+1], request[MAXLINE+1], buff[MAXLINE];
	
	//CREATE SOCKET
	puts("create socket");
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
	puts("bind");
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		perror("bind error");

	//LISTEN
	puts("listen");
	if(listen(listenfd, LISTENQ) < 0)
		perror("listen error");

	//INF LOOP 1
	for(;;){

		//ACCEPT
		puts("waiting to accept() client");
		connfd = accept(listenfd, (SA*) NULL, NULL);
		if (connfd < 0)
			perror("accept error");


		//INF LOOP 2
		for(;;){
			//READ
			puts("reading message....\n");
			while( (n = read(connfd, recvline, MAXLINE)) > 0) {
				recvline[n] = 0;	// null terminate
				//print to console
				if (fputs(recvline, stdout) == EOF)
					perror("fputs error");
				//print to char request[]
				if(sprintf(request, "%s", recvline) < 0)	//using sprintf (not snprintf) creates a potential vulnerability.
					perror("sprintf error");
				
			} 
			fputs("\n", stdout);	//newline

			//WRITE
			puts("sending response....");
			snprintf(buff, sizeof(buff), "msg recieved\r\n");
			write(connfd, buff, strlen(buff));

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
