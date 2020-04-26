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

void perror_exit(const char *str)
{
	cleanup();
	perror(str);
	exit(EXIT_FAILURE);
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
		perror_exit("bind");
	}

	if (signal(SIGINT, sighandler) == SIG_ERR) {
		perror_exit("signal");
	}

	while (1) {
		char msg[BUF_SIZE];
		const ssize_t bytes = recv(sockfd, msg, sizeof msg, 0);
		if (bytes == -1) {
			perror_exit("recv");
		}
		msg[bytes] = '\0';

		const time_t timer = time(NULL);
		printf("[%.19s] receive message: %s\n", ctime(&timer), msg);
	}
}
