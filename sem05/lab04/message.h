#ifndef MESSAGE_H_
#define MESSAGE_H_

void receive_message_on_child(int i, int pipefd[2]);
void send_message_to_child(int i, int pipefd[2]);

#endif  // MESSAGE_H_
