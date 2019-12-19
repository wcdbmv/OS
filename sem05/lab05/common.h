#ifndef COMMON_H_
#define COMMON_H_

#include <sys/sem.h>
#include <sys/shm.h>

int randint(int a, int b);

void pexit(const char *msg);
void safe(int returns, const char *msg);
void safe_semop(int semid, struct sembuf *sops, size_t nsops);

int safe_semget(key_t key, int nsems, int semflg);
int safe_shmget(key_t key, size_t size, int shmflg);
void *safe_shmat(int shmid, const void *shmaddr, int shmflg);

int fork_children(int children_count, void (*on_child)(int id));
void wait_children(int children_count);

#endif  // COMMON_H_
