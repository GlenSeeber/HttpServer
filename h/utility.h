#include <stdio.h>
#include <stdlib.h>		//strtol()
#include <string.h>
#include <unistd.h>		//read()

#define HEADER 4
#define MAXLINE	4096

#define SA 		struct sockaddr
#define PORT 	80
#define LISTENQ	1024


//--WRITE--
void makeHeader(char *str, char msg[]){

	//WRITE
	int32_t msgLen = strlen(msg);
	char myHeader[HEADER];

	//i is number of chars printed
	int i = sprintf(myHeader, "%d", msgLen);
	
	//append tabs to pad the header			
	for ( ; i < HEADER; ++i){
		myHeader[i] = '\t';
	}
	//set buff as the header (myHeader) plus the actual message (msg)
	sprintf(str, "%s%s", myHeader, msg);	
}

//--READ--
int readHeaderMsg(int connfd, char request[]){
	
	char 		c_recvLen[HEADER], recvline[MAXLINE+1];
	int 		n, recvLen;
	long int	l_recvLen;

	//READ

	//read just the header of the message
	n = 0;
	//only read until you have read all the bytes in the header
	while(n < HEADER) {
		//add the amount of bits read to n
		if ( (n += read(connfd, c_recvLen, HEADER-n)) == 0){
			fprintf(stderr, "error: no bytes to read");
			return -1;
		}
		
		char *ptr;
		l_recvLen = strtol(c_recvLen, &ptr, 10);
		recvLen = (int) l_recvLen;	//recvLen is the amount of bytes in the actual message
	}
	//read only recvLen bytes (which was sent to us in the header)
	n = 0;
	while(n < recvLen) {
		//add the amount of bits read to n
		if( (n += read(connfd, recvline, recvLen-n)) == 0){
			fprintf(stderr, "error: no bytes to read");
			return -1;
		}	
		
		recvline[n] = 0;	// null terminate
		//print to console
		/*if (fputs(recvline, stdout) == EOF)
			perror("fputs error");
		*/
		//print to char request[]
		if(sprintf(request, "%s", recvline) < 0){	//using sprintf (not snprintf) creates a potential vulnerability.
			perror("sprintf error");
			return -1;
		}
		return 0;
	} 
}
