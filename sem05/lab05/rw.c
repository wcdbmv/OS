#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <uinstd.h>
#include "common.h"

#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define AR 0  /* active reader  */
#define WW 1  /* waiting writer */
#define AW 2  /* active writer  */
#define WR 3  /* waiting reader */

struct sembuf writer_start[4] = {{WW,  1, 0}, {AR, 0, 0}, {AW, -1, 0}, {WW, -1, 0}};
struct sembuf writer_stop [1] = {{AW,  1, 0}};
struct sembuf reader_start[5] = {{WR,  1, 0}, {AW, 0, 0}, {WW,  0, 0}, {AR,  1, 0}, {WR, -1, 0}};
struct sembuf reader_stop [1] = {{AR, -1, 0}};

void start_write(void) {
	ssemop(sem_id, writer_start, ARRAY_SIZE(writer_start));
}

void stop_write(void) {
	ssemop(sem_id, writer_stop, ARRAY_SIZE(writer_stop));
}

void start_read(void) {
	ssemop(sem_id, reader_start, ARRAY_SIZE(reader_start));
}

void stop_read(void) {
	ssemop(sem_id, reader_stop, ARRAY_SIZE(reader_stop));
}

void writer(int id) {
	for (;;) {
		sleep(randint(1, 3));
		start_write();

		//

		stop_write();
	}
}

int main(void) {

}
