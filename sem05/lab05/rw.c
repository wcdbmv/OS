#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SAW 0
#define SAR 1
#define SWW 2
#define SWR 3

#define P -1
#define V  1

#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define N 5

#define PERMS S_IRWXU | S_IRWXG | S_IRWXO

struct sembuf canread   [3] = {{SAW,  0, 0}, {SWW, 0, 0}, {SWR, 1, 0}};
struct sembuf startread [2] = {{SWR, -1, 0}, {SAR, 1, 0}};
struct sembuf stopread  [1] = {{SAR, -1, 0}};
struct sembuf canwrite  [3] = {{SAR,  0, 0}, {SAW,  0, 0}, {SWW, 1, 0}};
struct sembuf startwrite[2] = {{SAW,  1, 0}, {SWW, -1, 0}};
struct sembuf stopwrite [1] = {{SAW, -1, 0}};

int sem_id = -1;
int shm_id = -1;

int *shm = NULL;

void start_read(void) {
	safe_semop(sem_id, canread, ARRAY_SIZE(canread));
	safe_semop(sem_id, startread, ARRAY_SIZE(startread));
}

void stop_read(void) {
	safe_semop(sem_id, stopread, ARRAY_SIZE(stopread));
}

void start_write(void) {
	safe_semop(sem_id, canwrite, ARRAY_SIZE(canwrite));
	safe_semop(sem_id, startwrite, ARRAY_SIZE(startwrite));
}

void stop_write(void) {
	safe_semop(sem_id, stopwrite, ARRAY_SIZE(stopwrite));
}

void reader(int id) {
	for (int i = 0; i < N; ++i) {
		sleep(randint(1, 3));
		start_read();

		printf("[on reader][#%d][pid %d] reads value %d\n", id, getpid(), *shm);

		stop_read();
	}
}

void writer(int id) {
	for (int i = 0; i < N; ++i) {
		sleep(randint(1, 2));
		start_write();

		++*shm;
		printf("[on writer][#%d][pid %d] writes value %d\n", id, getpid(), *shm);

		stop_write();
	}
}

void init_sem(void) {
	sem_id = safe_semget(IPC_PRIVATE, 5, IPC_CREAT | PERMS);
	safe(semctl(sem_id, SAW, SETVAL, 0), "semctl");
	safe(semctl(sem_id, SAR, SETVAL, 0), "semctl");
	safe(semctl(sem_id, SWR, SETVAL, 0), "semctl");
	safe(semctl(sem_id, SWW, SETVAL, 0), "semctl");
}

void init_shm(void) {
	shm_id = safe_shmget(IPC_PRIVATE, sizeof (int), IPC_CREAT | PERMS);
	shm = safe_shmat(shm_id, 0, 0);
}

void clear(void) {
	safe(shmctl(shm_id, IPC_RMID, NULL), "shmctl");
	safe(semctl(sem_id, SAW, IPC_RMID, 0), "semctl");
}

int main(void) {
	srand(time(NULL));

	init_sem();
	init_shm();

	int children_count = fork_children(WRITERS_COUNT, writer);
	children_count    += fork_children(READERS_COUNT, reader);

	wait_children(children_count);
	clear();
}
