#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

pthread_mutex_t lockReader, lockWriter;
pthread_cond_t cond;
int readSpace = 0;
int readers = 0;
int writers = 0;

void* reader(void *readerid) {
    int tRed = (int) readerid;
    while(1){
        pthread_mutex_lock(&lockReader);
        while(writers < 0){
            pthread_cond_wait(&cond, &lockReader);
        }
        readers++;
        pthread_mutex_unlock(&lockReader);


        printf("Reader %d leyo: %d\n", tRed, readSpace);
        sleep(1);

        pthread_mutex_lock(&lockReader);
        readers--;
        if (readers == 0){
            pthread_cond_signal(&cond);
        }
        pthread_mutex_unlock(&lockReader);
        sleep(rand() % 3 + 1);
    }
}

void* writer(void *writerid){
    int wid = (int) writerid;
    while(1){
        pthread_mutex_lock(&lockReader);
        writers++;
        printf("Writer %d esta esperando...\n", wid);
        while (readers > 0){
            pthread_cond_wait(&cond, &lockReader);
        }
        pthread_mutex_unlock(&lockReader);
        
        pthread_mutex_lock(&lockWriter);

        readSpace++;
        printf("Writer %d escribio: %d\n", wid, readSpace);
        sleep(1);

        pthread_mutex_unlock(&lockWriter);

        pthread_mutex_lock(&lockReader);
        writers--;
        pthread_cond_broadcast(&cond);
        pthread_mutex_unlock(&lockReader);
    }
    
}

int main() {
    pthread_t treader[5], twriter[3];

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

    if (pthread_cond_init(&cond, NULL) != 0){
        printf("Condicional fallo");
        return 1;
    }

    // Init Threads
    for(int i = 0; i < 5; i++){
        if(pthread_create(&treader[i], NULL, reader, (void*)i)){
            printf("No se pudo crear le thread");
            return 1;
        }
    }

    for (int i = 0; i < 3;i++){
        if(pthread_create(&twriter[i], NULL, writer, (void*)i)){
            printf("No se pudo crear le thread");
            return 1;
        }
    }

    for(int i = 0; i<5;i++){
        pthread_join(treader[i], NULL);
    }

    for(int i = 0; i<3;i++){
        pthread_join(twriter[i], NULL);
    }

    pthread_exit(NULL);
    pthread_mutex_destroy(&lockReader);
    pthread_mutex_destroy(&lockWriter);

    
    return 0;
}