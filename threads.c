#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define SIZE 1000000
#define NUM_THREADS 6

typedef struct {
    int *vector;
    int start;
    int end;
    long long partial_sum;
} ThreadData;

void generate_random_vector(int vector[]) {
    for (int i = 0; i < SIZE; i++) {
        vector[i] = rand() % 100;
    }
}

long long sequential_sum(int vector[]) {
    long long sum = 0;
    for (int i = 0; i < SIZE; i++) {
        sum += vector[i];
    }
    return sum;
}

void *soma(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    long long sum = 0;

    for (int i = data->start; i < data->end; i++) {
        sum += data->vector[i];
    }

    data->partial_sum = sum;
    return NULL;
}

int main(void) {
    int *vector = malloc(SIZE * sizeof(int));
    if (vector == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para o vetor.\n");
        return 1;
    }

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];

    srand((unsigned int)time(NULL));
    generate_random_vector(vector);

    int chunk = SIZE / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].vector = vector;
        thread_data[i].start = i * chunk;

        if (i == NUM_THREADS - 1) {
            thread_data[i].end = SIZE;
        } else {
            thread_data[i].end = (i + 1) * chunk;
        }

        thread_data[i].partial_sum = 0;

        if (pthread_create(&threads[i], NULL, soma, &thread_data[i]) != 0) {
            fprintf(stderr, "Erro ao criar a thread %d.\n", i);
            free(vector);
            return 1;
        }
    }

    long long parallel_sum = 0;

    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            fprintf(stderr, "Erro ao aguardar a thread %d.\n", i);
            free(vector);
            return 1;
        }
        parallel_sum += thread_data[i].partial_sum;
    }

    long long sequential = sequential_sum(vector);

    printf("Soma paralela   : %lld\n", parallel_sum);
    printf("Soma sequencial : %lld\n", sequential);

    if (parallel_sum == sequential) {
        printf("Resultado correto: as somas sao iguais.\n");
    } else {
        printf("Erro: as somas sao diferentes.\n");
    }

    free(vector);
    return 0;
}