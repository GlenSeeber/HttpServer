#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include "h/utility.h"

int main(int argc, char *argv[]){	
	int					sockfd, n, err;
	char				reply[MAXLINE+1], recvline[MAXLINE + 1], buff[MAXLINE];
	struct sockaddr_in	servaddr;

	char				quit[] = "\r\n\r\n";

	if (argc != 2)
		fputs("usage: a.out <IPaddress>", stderr);


	//SOCKET
	puts("socket()");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0){
		perror("socket error");
	}

	//set values
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(PORT);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		fprintf(stderr, "inet_pton error for %s", argv[1]);

	//CONNECT
	puts("connect");
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0){
		perror("connect error");
		return 1;
	}
	//loop for server connection
	for(;;){
		//WRITE
		printf("[CLIENT] > ");
		char msg[MAXLINE-sizeof(quit)];
		char c;

		//empty the buff
		memset(buff, 0, sizeof(buff));

		//this just records stdin to msg[] until you hit enter
		for(int i = 0; (c = getchar()) != '\n'; ++i){
			msg[i] = c;
		}

		//print msg+quit onto the buff	
		sprintf(buff, "%s%s", msg, quit);
	
		printf("buff: {%s}\n", buff);
		
		//write to server
		if(write(sockfd, buff, strlen(buff)) != strlen(buff))
			perror("write error");


		//READ		
		n = 0;

		//empty the buff
		memset(buff, 0, sizeof(buff));
		

		//read bytes until we find '\n' or if we hit 50 total bytes.
		do{
			//buff+n so we don't overwrite previous reads.
			n += Read(sockfd, buff+n, 1);
		}while(buff[n-1] != '\n' && n < 50);
		
		printf("[SERVER] > %s\n", buff);	//print the message



		//logicgate determine if break
		if(1){
			break;
		}
	}

	//CLOSE
	puts("close");
	close(sockfd);
}
