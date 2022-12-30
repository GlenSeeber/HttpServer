#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define SA 		struct sockaddr
#define PORT 	13
#define MAXLINE	4096
#define LISTENQ	1024

#define HEADER 64


int main(int argc, char *argv[]){	
	int					sockfd, n, err;
	char				recvline[MAXLINE + 1], buff[MAXLINE];
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
		puts("send a message to the server\n> ");
		char msg[MAXLINE];
		scanf("%s", &msg);		
		int32_t msgLen = strlen(msg);
		char myHeader[HEADER];
		//i is the number of chars printed
		int i = sprintf(myHeader, "%d", msgLen);
		
		for ( ; i < HEADER; ++i){
			myHeader[i] = '\t';
		}	
		snprintf(buff, sizeof(buff), "%s%s", myHeader, msg);
		if(write(sockfd, buff, strlen(buff)) != strlen(buff))
			perror("write error");


		//READ
		puts("reading...");
		while ( (n = read(sockfd, recvline, MAXLINE)) > 0) {
			fputs("loop to read from server....\n", stdout);
			recvline[n] = 0;	/* null terminate */
			if (fputs(recvline, stdout) == EOF)
				fputs("fputs error", stderr);
		}	
		//logicgate determine if break
		if(1){
			puts("this should happen");
			break;
		}
	}

	//CLOSE
	puts("close");
	close(sockfd);
}
