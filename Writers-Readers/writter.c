#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lockReader, lockWriter;
int readSpace = 0;
int readers = 0;

void* reader(void *threadID) {
    int tID = (int) threadID;

    while (1){

        pthread_mutex_lock(&lockReader);
        readers++;
        if (readers == 1){
            pthread_mutex_lock(&lockWriter);
        }
        pthread_mutex_unlock(&lockReader);

        printf("Reader %d leyendo: %d\n", tID,readSpace);
        sleep(1);

        pthread_mutex_lock(&lockReader);
        readers--;
        if(readers == 0){
            pthread_mutex_unlock(&lockWriter);
        }
        pthread_mutex_unlock(&lockReader);
        sleep(rand() % 3 + 1);
    }
}

void* writer(void *writer) {
    int wid = (int) writer;
    while(1){
        pthread_mutex_lock(&lockWriter);

        printf("Antes %d\n", readSpace);
        readSpace++;
        printf("Escribio el Escritor %d: %d\n", wid, readSpace);
        sleep(1);

        pthread_mutex_unlock(&lockWriter);
        sleep(rand() % 3 + 1);
    }
}

int main(){
    //threads
    pthread_t treader[3], twriter[2];
    
    //init mutex
    if (pthread_mutex_init(&lockReader, NULL) != 0)
    {
        printf("Mutex fallo al inciar");
        return 1;
    }

    if (pthread_mutex_init(&lockWriter, NULL) != 0)
    {
        printf("Mutex fallo al inciar");
        return 1;
    }

    // Init Threads
    for(int i = 0; i < 3; i++){
        if(pthread_create(&treader[i], NULL, reader, (void*)i)){
            printf("No se pudo crear le thread");
            return 1;
        }
    }

    for (int i = 0; i < 2;i++){
        if(pthread_create(&twriter[i], NULL, writer, (void*)i)){
            printf("No se pudo crear le thread");
            return 1;
        }
    }

    for(int i = 0; i<3;i++){
        pthread_join(treader[i], NULL);
    }

    for(int i = 0; i<2;i++){
        pthread_join(twriter[i], NULL);
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&lockReader);
    pthread_mutex_destroy(&lockWriter);

    return 0;
}