#include <unistd.h>
#include <stdio.h>

void
Write(int fd, void *ptr, size_t nbytes)
{
	if (write(fd, ptr, nbytes) != nbytes)
		fprintf(stderr, "write error");
}
