#include <stdio.h>
#include <stdlib.h>

int main(){
	
	char *buff = malloc(32);
	sprintf(buff, "abc23\n here are some nums");

	char *p;
	
	long int b = 0;

	//	*(buff+n) == buff[n]
	//	buff[0] == 'a' == 97
	//	*(buff+0) == buff[0] == 97 == 'a'
	//	buff+0 != *(buff+0)
	// 	buff+0 == 0x55f9d0826208
	// *(buff+0) == 97 == 0x61 (hexidecimal)
	

	

	b = strtol(*(buff+3), &p, 10);
	
	printf("result: %d\n", b);
	

	return 0;
}
