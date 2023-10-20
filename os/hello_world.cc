#include <stdio.h>
#include "thread.h"

const int N_Threads = 10;
static thread_t threads[N_Threads];

void go(int n) {
	printf("Hello from thread %d\n", n);
	thread_exit(100 + n);
	// Not reached
}

int main(int argc, char **argv) {
	long exit_value;

	for (int i = 0; i < N_Threads; ++i) {
		thread_create(&(threads[i]), &go, i);
	}

	for (int i = 0; i < N_Threads; ++i) {
		exit_value = thread_join(threads[i]);
		printf("Thread %d returned with %ld\n", i, exit_value);
	}
	printf("Main thread done.\n");
	return 0;
}
