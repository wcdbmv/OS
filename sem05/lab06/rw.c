#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define N 5

#define SLEEP_TIME 200

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;

HANDLE writers[WRITERS_COUNT];
HANDLE readers[READERS_COUNT];

volatile LONG active_readers_count = 0;
bool writing = false;

int value = 0;

volatile LONG waiting_writers_count = 0;
volatile LONG waiting_readers_count = 0;


void start_read(void) {
	InterlockedIncrement(&waiting_readers_count);
	if (writing || WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0) {
		WaitForSingleObject(can_read, INFINITE);
	}

	WaitForSingleObject(mutex, INFINITE);
	InterlockedDecrement(&waiting_readers_count);
	InterlockedIncrement(&active_readers_count);
	SetEvent(can_read);
}

void stop_read(void) {
	InterlockedDecrement(&active_readers_count);

	if (active_readers_count == 0) {
		SetEvent(can_write);
	}

	ReleaseMutex(mutex);
}

void start_write(void) {
	InterlockedIncrement(&waiting_writers_count);
	if (writing || active_readers_count > 0) {
		WaitForSingleObject(can_write, INFINITE);
	}

	InterlockedDecrement(&waiting_writers_count);
	WaitForSingleObject(mutex, INFINITE);
	writing = true;
}

void stop_write(void) {
	writing = false;

	if (WaitForSingleObject(can_read, 0) == WAIT_OBJECT_0) {
		SetEvent(can_read);
	} else {
		SetEvent(can_write);
	}

	ReleaseMutex(mutex);
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpointer-to-int-cast"

DWORD WINAPI writer(LPVOID lpParams) {
	for (int i = 0; i < N; ++i) {
		start_write();

		++value;
		printf("[on writer][#%d] writes value '%d'\n", (int) lpParams, value);

		stop_write();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

DWORD WINAPI reader(LPVOID lpParams) {
	while (value < WRITERS_COUNT * N) {
		start_read();

		printf("[on reader][#%d] reads value '%d'\n", (int) lpParams, value);

		stop_read();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

#pragma GCC diagnostic pop

int init_handles(void) {
	if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL) {
		perror("CreateMutex");
		return EXIT_FAILURE;
	}

	if ((can_read = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
		perror("CreateEvent");
		return EXIT_FAILURE;
	}

	if ((can_write = CreateEvent(NULL, FALSE, TRUE, NULL)) == NULL) {
		perror("CreateEvent");
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"

int create_threads(HANDLE *threads, int threads_count, DWORD (*on_thread)(LPVOID)) {
	for (int i = 0; i < threads_count; ++i) {
		if ((threads[i] = CreateThread(NULL, 0, on_thread, (LPVOID) i, 0, NULL)) == NULL) {
			perror("CreateThread");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

#pragma GCC diagnostic pop

int main(void) {
	setbuf(stdout, NULL);

	int rc = EXIT_SUCCESS;

	if ((rc = init_handles()) != EXIT_SUCCESS
	 || (rc = create_threads(writers, WRITERS_COUNT, writer)) != EXIT_SUCCESS
	 || (rc = create_threads(readers, READERS_COUNT, reader)) != EXIT_SUCCESS) {
		return rc;
	}

	WaitForMultipleObjects(WRITERS_COUNT, writers, TRUE, INFINITE);
	WaitForMultipleObjects(READERS_COUNT, readers, TRUE, INFINITE);

	CloseHandle(mutex);
	CloseHandle(can_read);
	CloseHandle(can_write);

	return rc;
}
