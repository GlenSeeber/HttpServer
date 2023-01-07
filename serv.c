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

	char				CRLF[] = "\r\n";
	
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

			char quit[] = "\r\n\r\n";
			
			//quit = "\r\n\r\n"
			sprintf(quit, "%s%s", CRLF, CRLF);
			
			int iter = 0;
			int quitCheck = 0;
			//read bytes until we find "\r\n\r\n" or if we hit MAXLINE total bytes.
			do{
				//read from fd
 				int err = Read(connfd, buff+n, 1);
				n += err;
				if(err < 0){
					break;
				}
			

				//checks if you have the quit sequence at the end of ur buffer
				if (n >= 4){
				//	printf("// \n\n");
					for(int i = 0; i < 4; ++i){
						//iterate backwards through each string 4 chars to check if they're the same
				//		printf("buff[n-(i+1)]: %d, %d\nquit[sizeof(quit)-(i+2)]: %lu, %d\n.\n", n-(i+1), buff[n-(i+1)], sizeof(quit)-(i+2), quit[sizeof(quit)-(i+2)]);
						if (buff[n-(i+1)] != quit[sizeof(quit)-(i+2)]){
							break;	//if any char is wrong, exit the for loop
						}
						//if characters have been correct 4 times, we can quit
						else if (i == 3){
							quitCheck = 1;
						}
					}
				}
				//actually quit reading now
				if (quitCheck == 1){
					break;
				}

			/*	printf("checking quit sequence");
				//check if quit sequence was found
				if (iter == sizeof(quit-1)){
					break;
				}
			//	printf("sequence not found, checking buff for sequence");
				//check latest character to quit[iter] 4 times.
				//if they all match, you've found the quit sequence
				if(buff[n-1] == quit[iter]){
					++iter;
					continue;
				}
				//if they ever don't match, reset iter
				iter = 0;
			*/
			}while(n <= MAXLINE && quitCheck != 1);

			//print what we got
			printf("[CLIENT]: \n\n%s[SERVER DESIGNATED END]\n\n", buff);
			
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
