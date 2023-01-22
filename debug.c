#include <stdio.h>
#include "h/utility.h"

int main(int argc, char **argv){
	char *buff = "POST / HTTP/1.1\r\nHost: localhost\r\nUser-Agent: curl/7.68.0\r\nAccept: */*\r\nContent-Length: 7\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\n";

	char keyword[]	= "Content-Length: ";



	int found = -1;

	int i = 87;

//	found = substring(buff, keyword, i);

	char *p;
	
	long n = 0;

	printf("buff: %p, buff[1]: %p, buff[i]: %p\n", buff, buff[1], buff[i]);

	printf("*buff: %p, *(buff+1): %p, *buff+1: %p, buff+1: %p\n", *buff, *(buff+1), *buff+1, buff+1);

	printf("%c\n", *(buff+i));

	n = strtol(buff+i, &p, 10);	

	printf("result: %d\n", n);

	return 0;
}
