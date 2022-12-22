#include <stdio.h>
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


int main(int argc, char *argv[]){	
	int					sockfd, n;
	char				recvline[MAXLINE + 1], buff[MAXLINE];
	struct sockaddr_in	servaddr;

	if (argc != 2)
		fputs("usage: a.out <IPaddress>", stderr);


	//SOCKET
	puts("socket()");
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	//set values
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port   = htons(13);	/* daytime server */
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		fprintf(stderr, "inet_pton error for %s", argv[1]);

	//CONNECT
	puts("connect");
	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		puts("connect error");

	for(;;){
		//WRITE
		puts("write...");
		snprintf(buff, sizeof(buff), "hello, world");
		write(sockfd, buff, strlen(buff));

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
