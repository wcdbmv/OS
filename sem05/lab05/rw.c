#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "common.h"

#define SB  0
#define SW  1
#define SAR 2
#define SWW 3
#define SWR 4

#define P -1
#define V  1

#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define N 5

#define PERMS S_IRWXU | S_IRWXG | S_IRWXO

// блокируется в ожидании момента, когда никто не пишет и нет ожидающих писателей
// увеличивает количество ждущих читателей
struct sembuf canread[3] = {{SW, 0, 0}, {SWW, 0, 0}, {SWR, 1, 0}};

// уменьшает количество ждущих писателей, увеличивает количество активных читателей
struct sembuf startread[2] = {{SWR, -1, 0}, {SAR, 1, 0}};

// уменьшает количество активных писателей
struct sembuf stopread[1] = {{SAR, -1, 0}};

// блокируется в ожидании момента, когда не будет активных читателей и активного писателя
// увеличивает количество ждущих писателей
struct sembuf canwrite[3] = {{SAR, 0, 0}, {SW, 0, 0}, {SWW, 1, 0}};

// уменьшаем количество ожидающих писателей, делаем активного писателя
// захватываем буфер
struct sembuf startwrite[3] = {{SWW, -1, 0}, {SW, 1, 0}, {SB, -1, 0}};

// писатель не активен, буфер не занят
struct sembuf stopwrite[2] = {{SW, -1, 0}, {SB, 1, 0}};

int sem_id = -1;
int shm_id = -1;

int *shm = NULL;

void reader(int id) {
	for (int i = 0; i < N; ++i) {
		sleep(randint(1, 2));
		safe_semop(sem_id, canread, ARRAY_SIZE(canread));
		safe_semop(sem_id, startread, ARRAY_SIZE(startread));

		printf("[on reader][#%d][pid %d] reads value %d\n", id, getpid(), *shm);

		safe_semop(sem_id, stopread, ARRAY_SIZE(stopread));
	}
}

void writer(int id) {
	for (int i = 0; i < N; ++i) {
		sleep(randint(1, 3));
		safe_semop(sem_id, canwrite, ARRAY_SIZE(canwrite));
		safe_semop(sem_id, startwrite, ARRAY_SIZE(startwrite));

		++*shm;
		printf("[on writer][#%d][pid %d] writes value %d\n", id, getpid(), *shm);

		safe_semop(sem_id, stopwrite, ARRAY_SIZE(stopwrite));
	}
}

void init_sem(void) {
	sem_id = safe_semget(IPC_PRIVATE, 5, IPC_CREAT | PERMS);
	safe(semctl(sem_id, SB,  SETVAL, 1), "semctl");
	safe(semctl(sem_id, SW,  SETVAL, 0), "semctl");
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
	safe(semctl(sem_id, SB, IPC_RMID, 0), "semctl");
}

int main(void) {
	srand(time(NULL));

	init_sem();
	init_shm();

	int children_count = fork_children(READERS_COUNT, reader);
	children_count    += fork_children(WRITERS_COUNT, writer);

	wait_children(children_count);
	clear();
}
