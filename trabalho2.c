#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#define QUANT_FILOSOFOS 5   // quantidade de filósofos
#define PENSANDO 0         // define valores para pensando, com fome e comendo
#define COM_FOME 1
#define COMENDO 2
#define FILOSOFO_DIREITA ((id_filosofo + QUANT_FILOSOFOS - 1) % QUANT_FILOSOFOS)
#define FILOSOFO_ESQUERDA ((id_filosofo + 1) % QUANT_FILOSOFOS)


int estado[QUANT_FILOSOFOS];            // acesso ao estado de cada um deles

sem_t mutex;                            // zona crítica 
sem_t mutex_filosofo[QUANT_FILOSOFOS];  // zona crítica de cada filósofo

pthread_t filosofos[QUANT_FILOSOFOS];   // todos os filósofos que vão comer

void verifica_garfos(int id_filosofo) {
    printf("O filosofo %d está querendo comer\n", id_filosofo);

    // verifica se o filosofo esta com fome e se nenhum dos vizinhos dele está comendo
    if((estado[id_filosofo] == COM_FOME) && (estado[FILOSOFO_ESQUERDA] != COMENDO) && (estado[FILOSOFO_DIREITA] != COMENDO)) {
        printf("O filósofo %d pode comer\n", id_filosofo);
        
        estado[id_filosofo] = COMENDO;  // como ele ganhou a vez de comer, ele passa a comer

        sem_post(&mutex_filosofo[id_filosofo]);  // libera os garfos para o filósofo
    }
    else {
        printf("O filosofo %d não conseguiu pegar o garfo\n", id_filosofo);
    }
}

void pegarGarfo(int id_filosofo) {
    sem_wait(&mutex);                       // entra na região crítica

    printf("O filosofo %d está com fome!\n", id_filosofo);
    estado[id_filosofo] = COM_FOME;

    verifica_garfos(id_filosofo);           // tenta pegar os garfos

    sem_post(&mutex);                       // sai da região crítica
    sem_wait(&mutex_filosofo[id_filosofo]); // bloqueia os garfos, que estão com o filosofo id_filosofo
}

void devolverGarfo(int id_filosofo) {
    sem_wait(&mutex);               // entra na região crítica

    printf("O filosofo %d está pensando...\n", id_filosofo);
    estado[id_filosofo] = PENSANDO;        // altera o status do filosofo

    verifica_garfos(FILOSOFO_ESQUERDA);    // verifica se o da esquerda pode comer
    verifica_garfos(FILOSOFO_DIREITA);     // verifica se o da direita pode comer

    sem_post(&mutex);                      // sai da região crítica
}


void pensar(int id_filosofo) {
    int tempo = (rand() % 10);   // sorteia um inteiro aleatório entre 1 e 10

    printf("Filósofo %d pensando por %d segundos\n", id_filosofo, tempo); 

    sleep(tempo);   // espera ele pensar
}

void comer(int id_filosofo) {
    int tempo = (rand() % 10);   // sorteio entre 1 e 10

    printf("Filósofo %d comendo por %d segundos\n", id_filosofo, tempo);

    sleep(tempo);   // espera comer
}

void *filosofo(void *arg) {
    int *id = (int *) (arg);     // cria o id do filósofo

    printf("O filósofo %d foi criado!\n", *id);

    while(1) {
        pensar(*id);             // penso, logo existo
        pegarGarfo(*id);         // pega os garfo
        comer(*id);              // come
        devolverGarfo(*id);      // devolve os garfo
    }

    pthread_exit((void*) 0); // usa para retornar um valor e ver se não deu erro na execucao
}

int main(void) {   
    int fil[5] = {0,1,2,3,4};  
    // inicia os semáforos
    sem_init(&mutex, 0, 1);

    for(int i = 0; i < QUANT_FILOSOFOS; i++) {
        sem_init(&mutex_filosofo[i], 0, 0);
    }

    // inicia as threads
    for(int i = 0; i < QUANT_FILOSOFOS; i++) {
        pthread_create(&filosofos[i], NULL, filosofo, &fil[i]);
        
    }
     for(int i = 0; i < QUANT_FILOSOFOS; i++) {
        pthread_join(filosofos[i], NULL);
     }

    // destrói os semaforos
    sem_destroy(&mutex);

    for(int i = 0; i < QUANT_FILOSOFOS; i++) {
        sem_destroy(&mutex_filosofo[i]);
    }

    return 0;
}
