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

//max size of a filename (not including its directory)
#define MAXFILE 50

//(move this to h/utility.h when you finish implimenting)
//saves 'size' bytes from the file designated at 'filename', saving them onto 'buff'. Reading in binary or not based on 'binary': 0 no, 1 yes.
int fileToString1(const char *filename, long size, char *buff){
	int		fd, n, err;
	FILE	*myFile;
	
	char	quit[3] = "\0", readMode[3] = "rb";
	
	//get file from filename
	myFile = fopen(filename, readMode);
	
	if (!myFile){
		myFile = fopen("files/404.html", "r");	//debug make sure you fix this eventually
	}
	
	n = 0;
	//read bytes until we find or if we hit MAXLINE total bytes.
	while(n <= size){
		//read from fd
		err = fRead(buff+n, 1, 1, myFile);
		n += err;
		if(err < 0){
			break;
		}
	
		//checks if you have the quit sequence at the end of ur buffer
		if (n >= 1 && buff[n-1] == 0 && 0){	//this should not run, but i might change my mind on implimentation back into serv.c
			break;
		}
	}
	
	return 0;
}


int main(int argc, char *argv[]){
	char 				quit[] = "\r\n\r\n", fileDir[] = "files";
	
	char	binaryExt[3][5] = {".ico", ".png", ".jpeg"};	//Make sure to update binaryCount when u update binaryExt
	int		binaryCount 	= 3;
	

	int					listenfd, connfd, n, recvLen, request;
	struct sockaddr_in	servaddr, client_addr;
	//SA					client_addr;
	char				recvline[MAXLINE+1], c_recvLen[HEADER];
//	char				msg[MAXLINE-strlen(quit)], buff[MAXLINE];
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

			//setup the buffer	
			char *buff = malloc(MAXLINE);	//change to whatever the chunk-size of the http request is eventually or something idk
			memset(buff, 0, MAXLINE);

			n = 0;
			//read bytes until we find "\r\n\r\n" or if we hit MAXLINE total bytes.
			while(n <= MAXLINE){
				//read from fd
 				int err = Read(connfd, &buff[n], 1);
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
			
			char 	target[MAXFILE+1], filename[MAXFILE+1+sizeof(fileDir)], version[5];
	

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

			//buffer overflow when writing name of target resource to filename
			if (strlen(fileDir) + strlen(target) - 1 > sizeof(filename)){
				status = 400;
				fprintf(stderr, "directory name-size overflow\n");
			}

			
			//CREATE RESPONSE
			
			//error page
			if (status >= 400){
				memset(target, 0, sizeof(target));
				sprintf(target, "/%d.html", status);
				fprintf(stderr, "status code %d", status);
			}
			
			//int binary = 0;
			free(buff);
			long fileSize = 0;
			char writeMode[4] = "w";
			//no errors, proceed to respond regularly
			switch(method){
				case 0:
					//put filepath+name on filename
					sprintf(filename, "%s%s", fileDir, target);
					
					printf("target: %s\n", target);

		/*				
					//"rb" for binary files, "r" for everything else	
					for (int i = 0; i < binaryCount; ++i){
						if(substring(filename, binaryExt[i], strlen(filename)-1)){
							binary = 1;
							sprintf(writeMode, "wb");
							printf("binary mode!\n");
							break;
						}
					}
		
					//saves contents of the file to msg, we're gonna send it to the client
					int err = fileToString(filename, msg, binary);
		*/

					
					
					//open file[] so we can get its size
					FILE *myFile = fopen(filename, "rb");
					//get size of myFile
					fseek(myFile, 0L, SEEK_END);
					fileSize = ftell(myFile);
					rewind(myFile);
					//close file
					fclose(myFile);
					
					
					//allocate enough room on the buff to hold the entire file
					buff = malloc(fileSize+5);

					//put the file on the buff
					memset(buff, 0, fileSize+5);
					int err = fileToString1(filename, fileSize, buff);
					// fileToString will return 404 if the file doesn't exist. This code is so our logs are accurate.
					if (err > 0){
						memset(target, 0, sizeof(target));
						sprintf(target, "%d.html", err);
						sprintf(writeMode, "w");	//this had "w\0" so i made it "w". might give me issues later??
					}

					printf(BLU "sending %s to client\n\n" RESET, target);
					break;
				case 1:
					printf("HEAD. Don't know what to do about it");
					break;
			}
			
			//WRITE
			//append \r\n to message
		//	sprintf(buff[fileSize], "\r\n");

			//print out server response
			if (0){//we just keep this off tbh
				printf("[SERVER]\n");
				for (int i = 0; i < fileSize; ++i){
					printf("%c", buff[i]);
				}
				printf("[SERVER-END]\n\n");
			}

			//write to file descriptor
			if(write(connfd, buff, fileSize) != fileSize)
				perror("write error");
		
			//free the buffer pointer
			free(buff);

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
