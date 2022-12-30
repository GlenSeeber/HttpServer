#include <stdio.h>

#include "utility.h"

int main(){
	char msg[] = "hello, world";	//12 chars -> 12 bytes
	printf("msg: %s\n", msg);

	char buff[32];

	makeHeader(buff, msg);

	printf("buff: %s\n", buff);
	
}
