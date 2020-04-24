#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include "socket.h"

int main(void)
{
	int rc = EXIT_SUCCESS;

	const int sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (sockfd == -1) {
		perror("socket(AF_UNIX, SOCK_DGRAM, 0)");
		return EXIT_FAILURE;
	}

	struct sockaddr addr;
	addr.sa_family = AF_UNIX;
	strncpy(addr.sa_data, SOCKET_NAME, ARRAY_SIZE(addr.sa_data));

	char msg[BUF_SIZE];
	snprintf(msg, ARRAY_SIZE(msg), "pid %d", getpid());
	if (sendto(sockfd, msg, strlen(msg), 0, &addr, sizeof addr) == -1) {
		fprintf(stderr, "sendto %s: %s\n", SOCKET_NAME, strerror(errno));
		rc = EXIT_FAILURE;
	}

	close(sockfd);
	return rc;
}
