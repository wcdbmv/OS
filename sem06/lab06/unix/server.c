#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "socket.h"

static int sockfd;

void cleanup(void)
{
	close(sockfd);
	unlink(SOCKET_NAME);
}

void sighandler(__attribute__((unused)) int  signum)
{
	cleanup();
	exit(EXIT_SUCCESS);
}

int main(void)
{
	if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr addr;
	addr.sa_family = AF_UNIX;
	strncpy(addr.sa_data, SOCKET_NAME, ARRAY_SIZE(addr.sa_data));
	if (bind(sockfd, &addr, sizeof addr) == -1) {
		cleanup();
		perror("bind");
		return EXIT_FAILURE;
	}

	if (signal(SIGINT, sighandler) == SIG_ERR) {
		cleanup();
		perror("signal");
		return EXIT_FAILURE;
	}

	while (1) {
		char msg[BUF_SIZE];
		const ssize_t bytes = recv(sockfd, msg, sizeof msg, 0);
		if (bytes == -1) {
			cleanup();
			perror("recv");
			return EXIT_FAILURE;
		}
		msg[bytes] = '\0';

		char time_buf[BUF_SIZE];
		const time_t timer = time(NULL);
		strftime(time_buf, sizeof time_buf, "%Y-%m-%d %H:%M:%S", localtime(&timer));
		printf("[%s] receive message: %s\n", time_buf, msg);
	}
}
