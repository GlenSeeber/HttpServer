#include <stdio.h>
#include "h/utility.h"


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
		printf("%d, ", buff[n]);
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


int main(){

	char file[] = "files/favicon.ico";
	FILE *outputFile;
	
	//open file[] so we can get its size
	FILE *myFile = fopen(file, "rb");
	//get size of myFile
	fseek(myFile, 0L, SEEK_END);
	long fileSize = ftell(myFile);
	rewind(myFile);
	//close file
	fclose(myFile);

	//allocate enough room on the buff to hold the entire file
	char *buff = malloc(fileSize);

	//put the file on the buff
	memset(buff, 0, fileSize);
	fileToString1(file, fileSize, buff);

//	printf("[MSG]:\n%s[ENDMSG]", buff);


	printf("[MSG]:\n");	
	//print message as string
	for (int i = 0; i < fileSize; ++i){
		printf("%c", buff[i]);
		
		if (buff[i] == 0){
			break;
		}
	}
	printf("[END-MSG]\n");
	
	outputFile = fopen("files/favicon2.ico", "wb+");
	fwrite(buff, 1, fileSize, outputFile);
	fclose(outputFile);

	free(buff);
	
	return 0;
}
