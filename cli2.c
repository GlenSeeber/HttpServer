#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#include "utility.h"

int main(int argc, char *argv[]){	
	int					sockfd, n, err;
	char				reply[MAXLINE+1], recvline[MAXLINE + 1], buff[MAXLINE];
	struct sockaddr_in	servaddr;

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

	for(;;){
		//WRITE
		printf("[CLIENT] > ");
		char msg[MAXLINE];
		char c;
		//this just records stdin to msg[] until you hit enter
		for(int i = 0; (c = getchar()) != '\n'; ++i){
			msg[i] = c;
		}
		
		//puts header then msg[] onto buff[]
		makeHeader(buff, msg);

		//write to server
		if(write(sockfd, buff, strlen(buff)) != strlen(buff))
			perror("write error");


		//READ
		//parse through header and read the message from server
		readHeaderMsg(sockfd, reply);

		printf("[SERVER] > %s\n", reply);	//print the message

		/*
		while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
			fputs("loop to read from server....\n", stdout);
			recvline[n] = 0;	// null terminate
			if (fputs(recvline, stdout) == EOF)
				fputs("fputs error", stderr);
		}*/	

		//logicgate determine if break
		if(0){
			break;
		}
	}

	//CLOSE
	puts("close");
	close(sockfd);
}
