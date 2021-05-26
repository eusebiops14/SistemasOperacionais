#include<stdio.h>
#include<unistd.h>
#include<strings.h>
#include<sys/wait.h>
#include<string.h>


#define STR_LEN 100

int main(void) {
    int fd[2];
    pid_t child;

    if (pipe(fd) < 0 ) {
        perror("pipe()");
        return -1;
    }

    child = fork(); //cria processo filho- suspende o processo pai e coloca o processo filho imediatamente em execução

    if (child ==0) { //filho le no 0
        char msg[STR_LEN];
        close(fd[1]); //fd[1]-descritor de escrita permite apenas a escrita é fechado pois processo filho vai apenas ler e nao pode escrever. PRocesso filho fecha fd[1]
        bzero(msg,STR_LEN);
        if (read(fd[0],msg, STR_LEN) > 0) {
            printf("Filho (PID=$%ld) leu: '%s'\n", (long)getpid(),msg);
        }
        printf("Filho terminando...dormindo 5s. \n");
        sleep(3);
        return 0;
    }else{ //pai escreve no fd[1]
        char pa_msg[STR_LEN];
        int status;
        close(fd[0]); //fd[0]- descritor permite apenas leitura e por isso é fechado pelo processo pai
        bzero(pa_msg,STR_LEN); //funcao bzero zera a região de memoria com inicio em pa_msg até STR_LEN
        printf("Pai (PID=%ld) escrevendo para o filho...\n",(long)getpid());
        sprintf(pa_msg, "Filho, preste atenção!.\n");
        write(fd[1], pa_msg,strlen(pa_msg));//strlen(pa_msg) é o comprimento da mensagem escrita pela funcao write
        waitpid(-1,&status,0);// System Call waitpid aguarda qualquer processo filho terminar. Ela é bloqueante
        printf("Pai: filho terminou...\n");
    }
    return 0;
}