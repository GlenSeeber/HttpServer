#include <stdio.h>
#include <stdlib.h>		//strtol()
#include <string.h>
#include <unistd.h>		//read()

#define HEADER 4
#define MAXLINE	4096

#define SA 		struct sockaddr
#define PORT 	80
#define LISTENQ	1024


int Read(int fd, void *buf, size_t count){
	int n;

	if( (n = read(fd, buf, count)) == 0){
		//we return an error (because we're out of bytes to read)
		fprintf(stderr, "error: no bytes to read\n");
		return -1;
	}
	//read() returns an error
	else if (n < 0){
		perror("read error");
		return -1;
	}
	return n;
}


int Write(int fd, const void *buf, size_t count){
	if(write(fd, buf, count) < 0)
		perror("write error");
}

//takes an html file, reads through it until the closing </html> tag, and saves it to char buff[]
void fileToString(const char *filename, char buff[]){
	int		fd, n, err;
	FILE	*myfile;

	char	quit[] = "</html>";

	//get file from filename
	myfile = fopen(filename, "r");
	//convert that to a file descriptor
	fd = fileno(myfile);

	//Now read the fd onto a char[]
	
	n = 0;
	//empty the buff
	memset(buff, 0, sizeof(buff));

	
	//read bytes until we find "</html>" or if we hit MAXLINE total bytes.
	while(n <= MAXLINE){
		//read from fd
		err = Read(fd, buff+n, 1);
		n += err;
		if(err < 0){
			break;
		}
	
		//checks if you have the quit sequence at the end of ur buffer
		if (n >= strlen(quit) && substring(buff, quit, n-1)){
			break;
		}
	}


}

//checks if a string (search) contains a substring (keyword), with the last letter being at lastIndex.
//returns 1 for true, 0 for false, and -1 on error
int substring(char search[], char keyword[], int lastIndex){

//	printf("search: %s\nkeyword: %s\n\n", search, keyword);
	
//	printf("lastIndex: %d\nstrlen(keyword): %ld\n", lastIndex, strlen(keyword));
	
	//prevents an array error
	if (lastIndex+1 >= strlen(keyword)){
		//iterate backwards through each char in search[] and keyword[]
		for(int i = 0; i < strlen(keyword); ++i){
//			printf("search: %d, %c \nkeyword: %lu, %c\n.\n", lastIndex-i, search[lastIndex-i], strlen(keyword)-(i+1), keyword[strlen(keyword)-(i+1)]);
			//iterate backwards through each string to check if they're the same
			if (search[lastIndex-(i)] != keyword[strlen(keyword)-(i+1)]){
				return 0;	//if any char is wrong, they're different
			}
			//if characters are correct and you're on the last loop, it means search contains keyword
			else if (i == strlen(keyword)-1){
				return 1;
			}
		}
	}
	else {
		fprintf(stderr, "error: lastIndex and strlen(keyword) cannot be resolved. Increase your lastIndex or decrease strlen(keyword)\n");
		return -1;
	}

}


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
			fprintf(stderr, "error: no bytes to read\n");
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
			fprintf(stderr, "error: no bytes to read\n");
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

