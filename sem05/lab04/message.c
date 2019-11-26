#include "message.h"
#include <stdio.h>
#include <unistd.h>
#include "display.h"

#define BUFFER_SIZE 32

void receive_message_on_child(int i, int pipefd[2]) {
	close(pipefd[1]);
	char buffer[BUFFER_SIZE];
	read(pipefd[0], buffer, sizeof buffer);
	display_on_child(i, "receives \"%s\"", buffer);
}

void send_message_to_child(int i, int pipefd[2]) {
	close(pipefd[0]);
	char message[] = "message to childxx";
	snprintf(message, sizeof message, "message to child%02d", i);
	write(pipefd[1], message, sizeof message);
	display_on_parent("send \"%s\"", message);
}
