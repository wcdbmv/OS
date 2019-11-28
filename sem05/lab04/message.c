#include "message.h"
#include <stdio.h>
#include <unistd.h>
#include "display.h"

#define BUFFER_SIZE 128

void receive_message_on_parent(int pipefd[2]) {
	close(pipefd[1]);
	char buffer[BUFFER_SIZE];
	read(pipefd[0], buffer, sizeof buffer - 1);
	display_on_parent("receives \"%s\"", buffer);
}

void send_message_to_parent(int i, int pipefd[2]) {
	close(pipefd[0]);
	char message[] = "mxx";
	snprintf(message, sizeof message, "m%02d", i);
	write(pipefd[1], message, sizeof message - 1);
	display_on_child(i, "send \"%s\"", message);
}
