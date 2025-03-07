#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lockReader, lockWriter;
pthread_mutex_t priorityLock;  // Mutex extra para controlar la prioridad de los escritores
int readSpace = 0;
int readers = 0;
int waiting_writers = 0;  // Contador de escritores en espera

void* reader(void *threadID) {
    int tID = (int) threadID;

    while (1) {
        // Evitar que los lectores entren si hay escritores esperando
        pthread_mutex_lock(&priorityLock);
        pthread_mutex_lock(&lockReader);
        readers++;
        if (readers == 1) {
            pthread_mutex_lock(&lockWriter);
        }
        pthread_mutex_unlock(&lockReader);
        pthread_mutex_unlock(&priorityLock);

        printf("Reader %d leyendo: %d\n", tID, readSpace);
        sleep(1);

        pthread_mutex_lock(&lockReader);
        readers--;
        if (readers == 0) {
            pthread_mutex_unlock(&lockWriter);
        }
        pthread_mutex_unlock(&lockReader);

        sleep(rand() % 3 + 1);
    }
}

void* writer(void *writer) {
    int wid = (int) writer;
    while(1) {
        // Indicar que un escritor está esperando
        pthread_mutex_lock(&lockReader);
        waiting_writers++;
        pthread_mutex_unlock(&lockReader);

        // Bloquear a los lectores y escribir
        pthread_mutex_lock(&lockWriter);

        pthread_mutex_lock(&lockReader);
        waiting_writers--;  // El escritor ya está ejecutando, reduce la cuenta de espera
        pthread_mutex_unlock(&lockReader);

        printf("Antes %d\n", readSpace);
        readSpace++;
        printf("Escribio el Escritor %d: %d\n", wid, readSpace);
        sleep(1);

        pthread_mutex_unlock(&lockWriter);

        sleep(rand() % 3 + 1);
    }
}

int main() {
    // Threads
    pthread_t treader[3], twriter[2];
    
    // Init mutexes
    pthread_mutex_init(&lockReader, NULL);
    pthread_mutex_init(&lockWriter, NULL);
    pthread_mutex_init(&priorityLock, NULL);

    // Init Threads
    for (int i = 0; i < 3; i++) {
        pthread_create(&treader[i], NULL, reader, (void*)i);
    }

    for (int i = 0; i < 2; i++) {
        pthread_create(&twriter[i], NULL, writer, (void*)i);
    }

    for (int i = 0; i < 3; i++) {
        pthread_join(treader[i], NULL);
    }

    for (int i = 0; i < 2; i++) {
        pthread_join(twriter[i], NULL);
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&lockReader);
    pthread_mutex_destroy(&lockWriter);
    pthread_mutex_destroy(&priorityLock);

    return 0;
}
