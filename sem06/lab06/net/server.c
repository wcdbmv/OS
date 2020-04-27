#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include "socket.h"

#define MAX_CLIENTS_COUNT 10

static int master_sd;
static int clients[MAX_CLIENTS_COUNT];

int perror_exit(const char *str)
{
	close(master_sd);
	perror(str);
	exit(EXIT_FAILURE);
}

void handle_connection(void)
{
	const int sd = accept(master_sd, NULL, NULL);
	if (sd == -1) {
		perror_exit("accept");
	}

	const time_t timer = time(NULL);
	for (int i = 0; i < MAX_CLIENTS_COUNT; ++i) {
		if (!clients[i]) {
			clients[i] = sd;
			printf("[%.19s] new connection: client %d (sd = %d)\n", ctime(&timer), i, sd);
			return;
		}
	}

	close(master_sd);
	fprintf(stderr, "[%.19s] reached MAX_CLIENTS_COUNT\n", ctime(&timer));
	exit(EXIT_FAILURE);
}

void handle_client(int i)
{
	char msg[BUF_SIZE];
	const ssize_t bytes = recv(clients[i], &msg, ARRAY_SIZE(msg), 0);
	const time_t timer = time(NULL);
	if (!bytes) {
		printf("[%.19s] client %d (sd = %d) disconnected\n", ctime(&timer), i, clients[i]);
		close(clients[i]);
		clients[i] = 0;
		return;
	}

	msg[bytes] = '\0';
	printf("[%.19s] receive message from client %d (sd = %d): %s\n", ctime(&timer), i, clients[i], msg);
}

int main(void)
{
	if ((master_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		return EXIT_FAILURE;
	}

	struct sockaddr_in addr = {
		.sin_family = AF_INET,
		.sin_addr.s_addr = INADDR_ANY,
		.sin_port = htons(SOCEKT_PORT)
	};

	if (bind(master_sd, (struct sockaddr *) &addr, sizeof addr) == -1) {
		perror_exit("bind");
	}

	if (listen(master_sd, MAX_CLIENTS_COUNT) == -1) {
		perror_exit("listen");
	}

	const time_t timer = time(NULL);
	printf("[%.19s] server is running on %s:%d\n", ctime(&timer), inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));

	while (1) {
		fd_set readfds;
		FD_ZERO(&readfds);

		FD_SET(master_sd, &readfds);

		int max_sd = master_sd;
		for (int i = 0; i < MAX_CLIENTS_COUNT; ++i) {
			if (clients[i] > 0) {
				FD_SET(clients[i], &readfds);
			}
			if (clients[i] > max_sd) {
				max_sd = clients[i];
			}
		}

		if (select(max_sd + 1, &readfds, NULL, NULL, NULL) == -1) {
			perror_exit("select");
		}

		if (FD_ISSET(master_sd, &readfds)) {
			handle_connection();
		}

		for (int i = 0; i < MAX_CLIENTS_COUNT; ++i) {
			if (clients[i] && FD_ISSET(clients[i], &readfds)) {
				handle_client(i);
			}
		}
	}
}
