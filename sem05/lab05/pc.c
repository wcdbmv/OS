#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SB 0  /* run access */
#define SE 1  /* buf empty  */
#define SF 2  /* buf full   */

#define P -1
#define V  1

#define PRODUCERS_COUNT 3
#define CONSUMERS_COUNT 5

#define N 100

#define PERMS S_IRWXU | S_IRWXG | S_IRWXO

int sem_id = -1;
int shm_id = -1;

char *shm       = NULL;
int  *shm_buff  = NULL;
int **shm_prod  = NULL;
int **shm_cons  = NULL;
int  *shm_value = NULL;

struct sembuf producer_start[2] = {{SB, P, 0}, {SE, P, 0}};
struct sembuf producer_stop [2] = {{SF, V, 0}, {SB, V, 0}};
struct sembuf consumer_start[2] = {{SB, P, 0}, {SF, P, 0}};
struct sembuf consumer_stop [2] = {{SE, V, 0}, {SB, V, 0}};

void producer(int id) {
	for (;;) {
		sleep(randint(1, 3));
		safe_semop(sem_id, producer_start, 2);
		if (*shm_prod == shm_buff + N) {
			safe_semop(sem_id, producer_stop, 2);
			break;
		}

		**shm_prod = *shm_value;
		printf("[on producer][#%d][pid %d] produces '%d'\n", id, getpid(), **shm_prod);
		++*shm_prod;
		++*shm_value;

		safe_semop(sem_id, producer_stop, 2);
	}
}

void consumer(int id) {
	for (;;) {
		sleep(randint(1, 5));
		safe_semop(sem_id, consumer_start, 2);
		if (*shm_cons == shm_buff + N) {
			safe_semop(sem_id, consumer_stop, 2);
			break;
		}

		printf("[on consumer][#%d][pid %d] consumes '%d'\n", id, getpid(), **shm_cons);
		++*shm_cons;

		safe_semop(sem_id, consumer_stop, 2);
	}
}

void init_sem(void) {
	sem_id = safe_semget(IPC_PRIVATE, 3, IPC_CREAT | PERMS);
	safe(semctl(sem_id, SB, SETVAL, 1), "semctl");
	safe(semctl(sem_id, SE, SETVAL, N), "semctl");
	safe(semctl(sem_id, SF, SETVAL, 0), "semctl");
}

void init_shm(void) {
	shm_id = safe_shmget(IPC_PRIVATE, 2 * sizeof (int *) + (N + 1) * sizeof (int), IPC_CREAT | PERMS);
	shm = safe_shmat(shm_id, 0, 0);

	shm_prod = (int **) shm;
	shm_cons = (int **) (shm + sizeof (int *));
	shm_value = (int *) (shm + 2 * sizeof (int *));
	shm_buff = (int *) (shm + 2 * sizeof (int *) + sizeof (int));

	*shm_prod = shm_buff;
	*shm_cons = shm_buff;
	*shm_value = 0;
}

void clear(void) {
	safe(shmctl(shm_id, IPC_RMID, NULL), "shmctl");
	safe(semctl(sem_id, SB, IPC_RMID, 0), "semctl");
}

void handler(int signum) {
	printf("\n[on parent] on handler [sig %d]\n", signum);
}

int main(void) {
	setbuf(stdout, NULL);
	setbuf(stderr, NULL);

	srand(time(NULL));

	init_sem();
	init_shm();

	int children_count = fork_children(PRODUCERS_COUNT, producer);
	children_count    += fork_children(CONSUMERS_COUNT, consumer);

	signal(SIGINT, handler);
	wait_children(children_count);
	clear();
}
