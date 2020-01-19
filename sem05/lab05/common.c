#include "common.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int randint(int a, int b) {
	assert(a <= b);
	return rand() % (b - a + 1) + 1;
}

void pexit(const char *msg) {
	perror(msg);
	exit(EXIT_FAILURE);
}

void safe(int returns, const char *msg) {
	if (returns == -1) {
		pexit(msg);
	}
}

void safe_semop(int semid, struct sembuf *sops, size_t nsops) {
	safe(semop(semid, sops, nsops), "semop");
}

int safe_semget(key_t key, int nsems, int semflg) {
	const int semid = semget(key, nsems, semflg);
	safe(semid, "semget");
	return semid;
}

int safe_shmget(key_t key, size_t size, int shmflg) {
	const int shmid = shmget(key, size, shmflg);
	safe(shmid, "shmget");
	return shmid;
}

void *safe_shmat(int shmid, const void *shmaddr, int shmflg) {
	void *shm = shmat(shmid, shmaddr, shmflg);
	if (shm == (void *) -1) pexit("shmat");
	return shm;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wimplicit-fallthrough"

int fork_children(int children_count, void (*on_child)(int id)) {
	int success_forks_count = 0;
	for (int i = 0; i < children_count; ++i) {
		switch (fork()) {
		case -1:
			pexit("fork");
		case 0:
			if (on_child) on_child(i);
			exit(EXIT_SUCCESS);
		default:
			++success_forks_count;
		}
	}

	return success_forks_count;
}

#pragma GCC diagnostic push

void wait_children(int children_count) {
	while (children_count--) {
		int status;
		const pid_t childpid = wait(&status);
		if (childpid == -1) {
			perror("wait");
			exit(EXIT_FAILURE);
		}

		if (WIFEXITED(status)) {
			printf("[on parent] child %d returns %d\n",
				childpid, WEXITSTATUS(status));
		} else if (WIFSIGNALED(status)) {
			printf("[on parent] child %d terminated with signal %d\n",
				childpid, WTERMSIG(status));
		} else if (WIFSTOPPED(status)) {
			printf("[on parent] child %d stopped due signal %d\n",
				childpid, WSTOPSIG(status));
		}
	}
}
