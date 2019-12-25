#include <stdio.h>
#include <stdbool.h>
#include <windows.h>

#define WRITERS_COUNT 3
#define READERS_COUNT 5

#define N 5

#define SLEEP_TIME 50

HANDLE mutex;
HANDLE can_read;
HANDLE can_write;

HANDLE writers[WRITERS_COUNT];
HANDLE readers[READERS_COUNT];

volatile LONG active_readers_count = 0;
bool writing = false;

int value = 0;


void start_read(void) {
	if (writing || WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0) {
		WaitForSingleObject(can_read, INFINITE);
	}

	WaitForSingleObject(mutex, INFINITE);
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
	if (writing || active_readers_count > 0) {
		WaitForSingleObject(can_write, INFINITE);
	}

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

DWORD WINAPI writer(__attribute__((unused)) LPVOID lpParams) {
	for (int i = 0; i < N; ++i) {
		start_write();

		++value;
		printf("[on writer][#%5ld] writes value '%d'\n", GetCurrentThreadId(), value);

		stop_write();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

DWORD WINAPI reader(__attribute__((unused)) LPVOID lpParams) {
	while (value < WRITERS_COUNT * N) {
		start_read();

		printf("[on reader][#%5ld] reads value '%d'\n", GetCurrentThreadId(), value);

		stop_read();
		Sleep(SLEEP_TIME);
	}

	return EXIT_SUCCESS;
}

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

int create_threads(void) {
	for (int i = 0; i < WRITERS_COUNT; ++i) {
		if ((writers[i] = CreateThread(NULL, 0, writer, NULL, 0, NULL)) == NULL) {
			perror("CreateThread");
			return EXIT_FAILURE;
		}
	}

	for (int i = 0; i < READERS_COUNT; ++i) {
		if ((readers[i] = CreateThread(NULL, 0, reader, NULL, 0, NULL)) == NULL) {
			perror("CreateThread");
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}

int main(void) {
	setbuf(stdout, NULL);

	int rc = EXIT_SUCCESS;

	if ((rc = init_handles()) != EXIT_SUCCESS) {
		return rc;
	}

	if ((rc = create_threads()) != EXIT_SUCCESS) {
		return rc;
	}

	WaitForMultipleObjects(WRITERS_COUNT, writers, TRUE, INFINITE);
	WaitForMultipleObjects(READERS_COUNT, readers, TRUE, INFINITE);

	CloseHandle(mutex);
	CloseHandle(can_read);
	CloseHandle(can_write);

	return rc;
}
