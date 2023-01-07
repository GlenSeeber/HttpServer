#include <stdio.h>

#include "h/utility.h"


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

int main(){
	char buff[MAXLINE+1];

	fileToString("files/index.html", buff);

	printf("buff:\n%s\n\n", buff);
	
	printf("last char: %d", buff[strlen(buff)-1]);
}
