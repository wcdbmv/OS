#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "children.h"

#define SB 0  /* run access */
#define SE 1  /* buf empty  */
#define SF 2  /* buf full   */

#define P -1
#define V  1

#define PRODUCERS_COUNT 3
#define CONSUMERS_COUNT 5

#define BUF_SIZE 100

int *shmbuf;
int *shmconsi;
int *shmprodi;

struct sembuf producer_start[2] = {{SE, P, 0}, {SF, P, 0}};
struct sembuf producer_stop [2] = {{SB, V, 0}, {SF, V, 0}};
struct sembuf consumer_start[2] = {{SB, P, 0}, {SF, P, 0}};
struct sembuf consumer_stop [2] = {{SE, V, 0}, {SF, V, 0}};

int sem_id = -1;

int randint(int a, int b) {
	assert(a <= b);
	return rand() % (b - a + 1) + 1;
}

void pexit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void ssemop(int semid, struct sembuf *sops, size_t nsops) {
	if (semop(semid, sops, nsops) == -1) {
		pexit("semop");
	}
}

void producer(int id) {
	for (;;) {
		sleep(randint(1, 3));
		ssemop(sem_id, producer_start, 2);

		shmbuf[*shmprodi] = *shmprodi;
		printf("[producer][#%d][pid %d] produce '%d'\n", id, getpid(), shmbuf[*shmprodi]);
		++*shmprodi;

		ssemop(sem_id, producer_stop, 2);
	}
}

void consumer(int id) {
	for (;;) {
		sleep(randint(1, 5));
		ssemop(sem_id, consumer_start, 2);

		printf("[consumer][#%d][pid %d] consume '%d'\n", id, getpid(), shmbuf[*shmconsi]);
		++*shmconsi;

		ssemop(sem_id, consumer_stop, 2);
	}
}

int main(void) {
	const int perms = S_IRWXU | S_IRWXG | S_IRWXO;

	const int shmid = shmget(IPC_PRIVATE, (BUF_SIZE + 1) * sizeof (int), IPC_CREAT | perms);
	if (shmid == -1) {
		pexit("shmget");
	}

	if (*(shmprodi = shmat(shmid, 0, 0)) == -1) {
		pexit("shmat");
	}

	shmbuf = shmprodi + 2 * sizeof (int);
	shmconsi = shmprodi + sizeof (int);

	*shmprodi = 0;
	*shmconsi = 0;

	if ((sem_id = semget(IPC_PRIVATE, 3, IPC_CREAT | perms)) == -1) {
		pexit("semget");
	}

	if (semctl(sem_id, SB, SETVAL, 0) == -1
	 || semctl(sem_id, SE, SETVAL, BUF_SIZE) == -1
	 || semctl(sem_id, SF, SETVAL, 1) == -1) {
		pexit("semctl");
	}

	srand(time(NULL));

	int children_count = fork_children(PRODUCERS_COUNT, producer);
	children_count    += fork_children(CONSUMERS_COUNT, consumer);
	wait_children(children_count);
}
