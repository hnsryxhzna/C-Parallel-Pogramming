#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 5

static pthread_mutex_t work_lock = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void* arg) {
    int index = *((int*) arg);

    pthread_mutex_lock(&work_lock);
    printf("Thread %d is working\n", index);
    printf("Thread %d is done\n", index);
    pthread_mutex_unlock(&work_lock);
    return NULL;
}

int main() {
    pthread_t threads[NUM_THREADS];
    int thread_args[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_args[i] = i;

        int return_code = pthread_create(&threads[i], NULL, thread_function, (void*) &thread_args[i]);

        if (return_code) {
            printf("Error in creating thread %d\n", return_code);
            exit(-1);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&work_lock);
    return 0;
}
