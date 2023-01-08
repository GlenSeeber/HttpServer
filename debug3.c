#include <stdio.h>

void here(char myArray[]){
	myArray[0] = 'a';
}


int main(){
	char testArray[] = "hello, world";
	//change testArray
	here(testArray);
	//print it
	printf("%s", testArray);
}
