#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "socket.h"

static char time_buf[BUF_SIZE];
const char *curtime(void)
{
	const time_t timer = time(NULL);
	strftime(time_buf, ARRAY_SIZE(time_buf), "%Y-%m-%d %H:%M:%S", localtime(&timer));
	return time_buf;
}

int main(void)
{
	const int master_sd = socket(AF_INET, SOCK_STREAM, 0);
	if (master_sd == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(SOCEKT_PORT)
	};

	if (connect(master_sd, (struct sockaddr *) &addr, sizeof addr) == -1) {
		perror("connect");
		return EXIT_FAILURE;
	}

	do {
		char msg[BUF_SIZE];
		snprintf(msg, ARRAY_SIZE(msg), "pid %d", getpid());
		if (sendto(master_sd, msg, strlen(msg), 0, (struct sockaddr *) &addr, sizeof addr) == -1) {
			perror("sendto");
			return EXIT_FAILURE;
		}

		printf("[%s] sent message: %s\n", curtime(), msg);
		printf("Press Enter to resend message...");
		getchar();
	} while (1);
}
