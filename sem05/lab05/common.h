#ifndef COMMON_H_
#define COMMON_H_

#include <sys/sem.h>

int randint(int a, int b);
void pexit(const char *msg);

void ssemop(int semid, struct sembuf *sops, size_t nsops);

int fork_children(int children_count, void (*on_child)(int id));
void wait_children(int children_count);

#endif  // COMMON_H_
