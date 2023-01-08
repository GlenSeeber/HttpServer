#include <stdio.h>

int main(){
	char myString[] = "hello, world";
	
	char *a = myString + 2;
	char *b = NULL;
	char *c = NULL + 1;

	printf("a: %p\nb: %p\nc: %p\n", a, b, c);

	printf("mySting: %s\na: %s\nb: %s\n", myString, a, b);

	printf("\nsizes:\na: %ld, b: %ld, c: %ld\n", sizeof(*a), sizeof(*b), sizeof(*c));

}
