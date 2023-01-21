#include <stdio.h>
#include <stdlib.h>		//strtol()
#include <string.h>
#include <unistd.h>		//read()

#define HEADER 4
#define MAXLINE	4096

#define SA 		struct sockaddr
#define PORT 	80
#define LISTENQ	1024

//colors
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m" 

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

int fRead(void *ptr, size_t size, size_t nmemb, FILE *stream){
	int n;

	if( (n = fread(ptr, size, nmemb, stream)) == 0){
		//we return an error (because we're out of bytes to read)
		fprintf(stderr, "error: no bytes to fread\n");
		return -1;
	}
	//read() returns an error
	else if (n < 0){
		perror("fread error");
		return -1;
	}
	return n;
}

int Fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream){
	if(fwrite(ptr, size, nmemb, stream) == 0){
		perror("fwrite error");
	}
}

//takes an html file, reads through it until the closing </html> tag, and saves it to char buff[]
int fileToString(const char *filename, char buff[], int binary){
	int		fd, n, err, output = 0;
	FILE	*myfile;
	
	char	quit[3] = "\0", readMode[3] = "r";

	//"rb" for binary files, "r" for everything else	
	if(binary){
		sprintf(readMode, "rb");
	}

	//get file from filename
	myfile = fopen(filename, readMode);
	
	if (!myfile){
		myfile = fopen("files/404.html", "r");	//debug make sure you fix this eventually
		output = 404;
	}
	
	//convert that to a file descriptor
	fd = fileno(myfile);		//you definetly don't need to do this, just use fread() instead of read()

	//Now read the fd onto a char[]
	
	n = 0;
	//empty the buff
	memset(buff, 0, sizeof(buff));

	//read bytes until we find or if we hit MAXLINE total bytes.
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
		if (n >= MAXLINE){	
			break;		//eventually, you should just make it send this out and then clear buff and print to this again or something idk
		}
	}
	
	return output;

}

//you need to wipe all allocated memory for the output vars using memset before calling this function
int parse(const char request[], char target[], int targetSize, char version[], int versionSize){
	//get the start and end of key bits of information (request target, http version, etc)
	// and output the actual info in seperate variables.	

	//printf("request:\n%s\n", request);

	//REQUEST TARGET (usually a page or file)
	const char *targetStart	= strchr(request, ' ');
	//adding *NULL + 1 creates a pointer somewhere we shouldn't be writing
	if (targetStart == NULL){
		fprintf(stderr, "error: targetStart should not be null");
		return -1;
	}	
	targetStart += 1;
	const char *targetEnd	= strchr(targetStart, ' ');
	int targetLen			= targetEnd - targetStart;	


	//HTTP VERSION
	const char *verStart	= strchr(targetEnd, '/') + 1;
	//prevent *NULL + 1
	if (verStart == NULL){
		fprintf(stderr, "error: verStart should not be null");
		return -1;
	}
	verStart += 1;
	const char *verEnd		= strchr(verStart, '\r');
	int verLen				= verEnd - verStart;



	//copy values onto strings (checking to prevent overflow)
	//request target
	if(targetSize <= targetLen){
		fprintf(stderr, "overflow error of 'target' variable\n");
		fprintf(stderr, "target: %d\ntargetLen: %d\n", targetSize, targetLen);
		return -1;
	}
	strncpy(target, targetStart, targetLen);

	//http version
	if (versionSize <= verLen){
		fprintf(stderr, "sizeof(version) error\n");
		return -1;
	}
	strncpy(version, verStart, verLen);			//http version
		
	return 0;
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

