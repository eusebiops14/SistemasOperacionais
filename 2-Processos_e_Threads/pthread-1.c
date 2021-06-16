#include<stdio.h>
#include<pthread.h>
#include<stdlib.h>

struct targs {
    int id;
    int usecs;
};

typedef struct targs targs_t;

void *t1(void *args){
    targs_t ta = *(targs_t *)args;
    int i = 0;
    int *ret; //PONTEIRO DE RETORNO
    ret = calloc(1,sizeof(int));
    *ret=100;
    while (1) {
        printf("thread=%d executando %d\n",ta.id, i++);
        usleep(ta.usecs);
        if ( i == 1000) {
            pthread_exit(ret);
        }
    }
    pthread_exit(ret); //retorna a variavel ret
}

int main(int argc, char **argcv) {
    
    pthread_t tid; //define uma variavel estatica do tipo thread
    targs_t ta = {0,10000};
    int *ret_val;
    pthread_create(&tid, NULL, t1,(void *)&ta); //: cria a thread
    pthread_join(tid,(void **)&ret_val); //: aguarda fim da execução da thread
    printf("main ret_val = %d \n", *ret_val);
    //pthread_exit : depois que a thread executou uma tarefa , ela retorna o valor a partir dessa primitiva

    return 0;
}