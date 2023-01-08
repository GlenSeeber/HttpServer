#include <stdio.h>

#include "h/utility.h"

#define MAXFILE 50	//how many characters can be contained in a file path


//you need to wipe all allocated memory for the output vars using memset before calling this function
int parseResource(const char request[], char target[], int targetSize, char version[], int versionSize){
	//get the start and end of key bits of information (request target, http version, etc)
	// and output the actual info in seperate variables.	

	printf("request:\n%s\n", request);

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
		fprintf(stderr, "sizeof(target) error\n");
		printf("target: '%d'\ntargetLen: %d\n", sizeof(target), targetLen);
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

int main(){
	char request[] = "GET /index.html HTTP/1.1\r\nHost: 192.168.1.45\r\n\r\n";
	char target[MAXFILE+1];
	char version[5];

	//empty our strings
	memset(target, 0, sizeof(target));
	memset(version, 0, sizeof(version));

	printf("target size: %d\n", sizeof(target));

	//parse
	if(parseResource(request, target, sizeof(target), version, sizeof(version)) < 0){
		perror("parse error");
	}

	//output
	printf("filename: '%s'\n", target);
	printf("HTTP/%s\n", version);


}


