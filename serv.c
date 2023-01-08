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

#define MAXFILE 50

int main(int argc, char *argv[]){
	char 				quit[] = "\r\n\r\n", fileDir[] = "files";
	
	char	binaryExt[3][5] = {".ico", ".png", ".jpeg"};	//Make sure to update binaryCount when u update binaryExt
	int		binaryCount = 3;
	

	int					listenfd, connfd, n, recvLen, request;
	struct sockaddr_in	servaddr, client_addr;
	//SA					client_addr;
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
	servaddr.sin_port        = htons(PORT);			//port 80 Web Server

	//BIND
	if(bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		perror("bind error");

	//LISTEN
	if(listen(listenfd, LISTENQ) < 0)
		perror("listen error");

	//Loop to ACCEPT clients
	for(;;){

		//ACCEPT
		puts("\nwaiting to accept() clients");
		connfd = accept(listenfd, (SA *) NULL, NULL);
		if (connfd < 0)
			perror("accept error");

		//print client ip address
/*		printf("IP Address: %s\n", inet_ntoa(client_addr.sin_addr));
		printf("Port:\t\t%d\n", (int) ntohs(client_addr.sin_port));
*/

		printf(GRN "\nACCEPTED CLIENT\n\n" RESET);
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
			printf("[CLIENT]: \n\n%s[CLIENT-END]\n\n", buff);
	
			//PROCESS REQUEST
			int 	status = -1, method = -1;
			
			char 	target[MAXFILE+1], myFile[MAXFILE+1+sizeof(fileDir)], version[5];
	

			//request method
			char methodList[9][8] = {"GET", "HEAD", "POST", "OPTIONS", "PUT", "DELETE", "CONNECT", "TRACE", "PATCH"};
			for (int i = 0; i < 9; ++i){
				if (substring(buff, methodList[i], strlen(methodList[i])-1 )){
					method = i;
					break;
				}
			}
			
			//target request and http version
			memset(target, 0, sizeof(target));
			memset(version, 0, sizeof(version));
			
			if (parse(buff, target, sizeof(target), version, sizeof(version)) < 0){
				fprintf(stderr, "parsing error\n");
			}
			
			
			//if you're resource is "/", make it "/index.html"
			if (strcmp(target, "/") == 0){
				memset(target, 0, sizeof(target));
				sprintf(target, "/index.html");
			}
			
			
			//if you aren't doing OPTIONS or CONNECT, you need to start with '/'
			else if (method == -1 || (target[0] != '/' && method != 3 && method != 6)){
				status = 400;	//bad request
				fprintf(stderr, "status 400, couldn't read method, or target request was incorrectly formatted\n");
			}
		

			//ERROR STATUS CODES		(more error codes under "CREATE RESPONSE")

			//buffer overflow when writing name of target resource to myFile
			if (strlen(fileDir) + strlen(target) - 1 > sizeof(myFile)){
				status = 400;
				fprintf(stderr, "directory name-size overflow\n");
			}

			
			//CREATE RESPONSE
			if (status >= 400){
				memset(target, 0, sizeof(target));
				sprintf(target, "/%d.html", status);
				fprintf(stderr, "status code %d", status);
			}

			int binary = 0;
			char writeMode[4] = "w";
			//no errors, proceed to respond regularly
			switch(method){
				case 0:
					//put filepath+name on myFile
					sprintf(myFile, "%s%s", fileDir, target);
					
					printf("target: %s\n", target);
						
					//"rb" for binary files, "r" for everything else	
					for (int i = 0; i < binaryCount; ++i){
						if(substring(myFile, binaryExt[i], strlen(myFile)-1)){
							binary = 1;
							sprintf(writeMode, "wb");
							printf("binary mode!\n");
							break;
						}
					}

					//saves contents of the file to msg, we're gonna send it to the client
					int err = fileToString(myFile, msg, binary);

					// fileToString will return 404 if the file doesn't exist. This code is so our logs are accurate.
					if (err > 0){
						memset(target, 0, sizeof(target));
						sprintf(target, "%d.html", err);
						sprintf(writeMode, "w\0");
					}

					printf(BLU "sending %s to client\n\n" RESET, target);
					break;
				case 1:
					printf("HEAD. Don't know what to do about it");
					break;
			}
			
			//WRITE
		
			//empty the buff
			memset(buff, 0, sizeof(buff));
			
			//append \r\n to message
			sprintf(buff, "%s\r\n", msg);
			
			//empty msg (so that it's already empty for next go around)
			memset(msg, 0, sizeof(msg));

			printf("[SERVER] > {%s}\n", buff);
			
			if(write(connfd, buff, strlen(buff)) != strlen(buff))
				perror("write error");
		

		/*
			FILE * connFile = fdopen(connfd, writeMode);

			if(fwrite(buff, 1, strlen(buff), connFile) != strlen(buff)){
				perror("write error");
			}
		*/

			//logic gate determines if we break the loop (leading to close()) or not
			if(1){
				break;
			}
			
		}

		//CLOSE
		close(connfd);
		puts(YEL "CONNECTION CLOSED\n\n" RESET);
	}
}
