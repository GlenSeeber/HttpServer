#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 9600
#define BUF_SIZE 12

ssize_t freadln(FILE *stream, char *buf, size_t max) {
	size_t n = 0;
	while (1) {
		if (n+1 >= max) {
			buf[n] = '\0';
			return n;
		}

		int ch = fgetc(stream);
		if (ch == EOF) {
			return n;
		} else if (ch < 0) {
			return ch;
		} else if (ch == '\r' || ch == '\n') {
			buf[n] = '\0';
			return n+1;
		} else {
			buf[n] = (char) ch;
			n++;
		}
	}
}

int main(int argc, char *argv[]) {
	int client_fd = socket(PF_INET, SOCK_STREAM, 0);
	if (client_fd < 0) {
		fprintf(stderr, "Cannot create socket\n");
		return 1;
	}

	struct sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	int ok = inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	if (!ok) {
		fprintf(stderr, "Cannot parse IP address\n");
		return 1;
	}

	int err = connect(client_fd, (struct sockaddr *) &server_addr, sizeof(server_addr));
	if (err != 0) {
		fprintf(stderr, "Cannot connect to server\n");
		return 1;
	}

	printf("Type messages to send to the server. End with an empty message.\n");

	while (1) {
		char buf[BUF_SIZE];
		ssize_t n = freadln(stdin, buf, sizeof(buf));
		if (n < 0) {
			fprintf(stderr, "Cannot read message to send from stdin\n");
			return 1;
		}
		if (n == 0) {
			break;
		}

		ssize_t written = write(client_fd, buf, n+1);
		if (written < 0) {
			fprintf(stderr, "Cannot write data\n");
			return 1;
		}

		printf("Sent: %s\n", buf);
	}

	printf("Closing connection\n");
	err = close(client_fd);
	if (err != 0) {
		fprintf(stderr, "Error while closing connection\n");
		return 1;
	}

	return 0;
}
