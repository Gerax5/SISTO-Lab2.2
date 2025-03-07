#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <semaphore.h>
#include <time.h>
#include <stdlib.h>

#define TAM_BUFFER 4
#define NUM_CONSUMERS 2
#define NUM_PRODUCERS 2
#define NUM_ITEMS 5


int buffer[TAM_BUFFER];
int in = 0;
int out = 0;
//contador de items consumidos y producidos
int count_produced = 0;

int count_consumed = 0;
//mutex para proteger el acceso al buffer
pthread_mutex_t mutex;

//semaforos para contar elementos llenos y el otro vacios
sem_t full;
sem_t empty;

void print_buffer() {
    //si tiene elementos validos se marca con *
    printf("buffer: [");
    for (int i = 0; i < TAM_BUFFER; i++) {
        // Verificar si la posición tiene un elemento valido
        int pos_valid = (in > out) ? (i >= out && i < in) : 
                        (in == out) ? 0 : (i >= out || i < in);
        
        if (i == in && i == out) {
            printf(" in=out(%d)%s", i, pos_valid ? "*" : "");
        } else if (i == in) {
            printf(" in(%d)%s", i, pos_valid ? "*" : "");
        } else if (i == out) {
            printf(" out(%d)%s", i, pos_valid ? "*" : "");
        } else {
            printf(" %d%s", i, pos_valid ? "*" : "");
        }
        
        // Mostrar el valor si la posición es valida
        if (pos_valid) {
            printf(":%d", buffer[i]);
        }
        
        if (i < TAM_BUFFER - 1) {
            printf(" , ");
        }
    }
    printf(" ]\n");

}


// producir item random
int produce_item() {
    return rand() % 100;
}

//consumir item
void consume_item(int item) {
    printf("item consumido: %d\n---------------\n", item);
}

void *productor(void *param) {

    int item;

    while (1) {
        //crear dato
        item = produce_item();
        printf("\nitem producido: %d\n", item);
        //esperar espacio
        sem_wait(&empty);
         //bloquear acceso
        pthread_mutex_lock(&mutex);
        //agregar a bufer
        buffer[in] = item;
        in = (in + 1) % TAM_BUFFER;
        //mostrar estado del buffer
        print_buffer();
        //contador
        count_produced++;
        //verificar el numero de items que sea menor o igual a count
        //se hace para guardar el valor del contador en el momento para usarla despues que se libera el mutex
        int should_exit = (count_produced >= NUM_ITEMS);
        // liberar
        pthread_mutex_unlock(&mutex);
        //senialar que hay un nuevo elemento
        sem_post(&full);
        //si se cumple salir
        if (should_exit) break;



    }
    return NULL;

}

void *consumidor(void *param) {
    int item;
    while (1) {
        // esperar dato disponible
        sem_wait(&full);
        // bloquear acceso
        pthread_mutex_lock(&mutex);
        //sacar dato de budder 
        item = buffer[out];
        out = (out + 1) % TAM_BUFFER;
        //mostrar estado del buffer
        print_buffer();
        ///contador
        count_consumed++;
        //verificar el numero de items 
        //se hace para guardar el valor del contador en el momento para usarla despues que se libera el mutex
        int should_exit = (count_consumed >= NUM_ITEMS);
        //liberar acceso
        pthread_mutex_unlock(&mutex);
        //senalar que hay un espacio vacio
        sem_post(&empty);
        //consumir dato
        consume_item(item);
        //si se cumple salir
        if (should_exit) break;

    }
    return NULL;
}


int main() {

    //iniciar semilla para los randoms
    srand(time(NULL));

    //hilos
    pthread_t producers[NUM_PRODUCERS];
    pthread_t consumers[NUM_CONSUMERS];

    //iniciar mutex para proteger al buffer del acceso compartido
    pthread_mutex_init(&mutex, NULL);
    //inicia el semadoro empty con el tam buffer que es la cantidad de espacios vacios
    sem_init(&empty, 0, TAM_BUFFER);
    //semaforo full que se va a ir llenando
    sem_init(&full, 0, 0);


    //creacion de los hilos productores
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_create(&producers[i], NULL, productor, NULL);
    }

    //creacion de los hilos consumidores
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_create(&consumers[i], NULL, consumidor, NULL);
    }

    // esperar a que los hilos productores terminen para 
    for (int i = 0; i < NUM_PRODUCERS; i++) {
        pthread_join(producers[i], NULL);
    }
    //esperar a que terminen los hilos consumidores
    for (int i = 0; i < NUM_CONSUMERS; i++) {
        pthread_join(consumers[i], NULL);
    }

    printf("\n--- Estado final del buffer ---\n");
    pthread_mutex_lock(&mutex);
    print_buffer();
    printf("Total de elementos producidos: %d\n", count_produced);
    printf("Total de elementos consumidos: %d\n", count_consumed);
    pthread_mutex_unlock(&mutex);

    //destruccion del mutex y los semaforos
    pthread_mutex_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);
    return 0;
}