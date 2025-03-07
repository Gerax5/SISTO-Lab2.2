#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define N 5 // Número de filósofos

pthread_mutex_t tenedores[N];
sem_t semaforo;

void comer(int id) {
    printf("Filósofo %d está comiendo.\n", id+1);
    sleep(rand() % 2 + 1); // Tiempo aleatorio entre 1 y 2 segundos
    printf("Filósofo %d terminó de comer.\n", id+1);
}

void* filosofo(void* num) {
    int id = *(int*)num;

    sem_wait(&semaforo); // Solicita permiso para evitar deadlock

    pthread_mutex_lock(&tenedores[id]); // Toma tenedor izquierdo
    pthread_mutex_lock(&tenedores[(id + 1) % N]); // Toma tenedor derecho
    // printf("Tenedores disponibles %d", &tenedores);
    comer(id);

    pthread_mutex_unlock(&tenedores[id]); // Suelta tenedor izquierdo
    pthread_mutex_unlock(&tenedores[(id + 1) % N]); // Suelta tenedor derecho

    sem_post(&semaforo); // Libera permiso

    pthread_exit(NULL); // Finaliza el hilo
}

int main() {
    int i;
    pthread_t filosofos[N];
    int ids[N];

    srand(time(NULL));

    sem_init(&semaforo, 0, N - 1); // Semáforo para permitir hasta N-1 filósofos
    for (i = 0; i < N; i++) {
        pthread_mutex_init(&tenedores[i], NULL);
    }

    for (i = 0; i < N; i++) {
        ids[i] = i;
        pthread_create(&filosofos[i], NULL, filosofo, &ids[i]);
    }

    for (i = 0; i < N; i++) {
        pthread_join(filosofos[i], NULL);
    }

    sem_destroy(&semaforo);
    for (i = 0; i < N; i++) {
        pthread_mutex_destroy(&tenedores[i]);
    }

    printf("\n Todos los filósofos han comido.\n");

    return 0;
}
