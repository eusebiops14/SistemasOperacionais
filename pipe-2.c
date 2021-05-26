#include<stdio.h>
#include<unistd.h>
#include<strings.h>
#include<sys/wait.h>
#include<string.h>


#define STR_LEN 100

int main(void) {
    int fd[2]; //pipe normal
    int ch_fd[2]; //pipe reverso- do filho para o pai
    pid_t child;

    if (pipe(fd) < 0 ) {
        perror("pipe()");
        return (-1);
    }

    if (pipe(ch_fd) < 0 ) {
        perror("pipe(ch_fd)");
        return (-1);//saida de erro voluntaria
    }


    child = fork(); //cria processo filho- suspende o processo pai e coloca o processo filho imediatamente em execução. O processo filho herda os descritores de arquivos abertos pelo pai


    if (child ==0) { //processo filho: le no 0
        char msg[STR_LEN];
        close(fd[1]); //fd[1]-descritor de escrita permite apenas a escrita é fechado pois processo filho vai apenas ler e nao pode escrever. PRocesso filho fecha fd[1]
        close(ch_fd[0]);// ch_fd[0] é o descitor de leitura do filho para o pai que é fechado pelo filho pois nao será usadp
        bzero(msg,STR_LEN);

        if (read(fd[0],msg, STR_LEN) > 0) {
   /*2*/printf("Filho (PID=$%ld) leu: '%s'\n", (long)getpid(),msg);
        }
        bzero(msg,STR_LEN);
        sprintf(msg,"Ok, papai!");
        write(ch_fd[1],msg,strlen(msg));
        printf("Filho terminando...dormindo 5s. \n");
        sleep(5);
        return 0;

    }else{ //processo pai: escreve no fd[1] e le no ch_fd[0]
        char pa_msg[STR_LEN];
        int status;
        close(fd[0]); //fd[0]- descritor permite apenas leitura e por isso é fechado pelo processo pai
        close(ch_fd[1]); //pai nao precisara realizar escrita no canal reverso com o filho
        bzero(pa_msg,STR_LEN); //funcao bzero zera a região de memoria com inicio em pa_msg até STR_LEN
   /*1*/printf("Pai (PID=%ld) escrevendo para o filho...dormindo (3s)\n",(long)getpid());
        sprintf(pa_msg, "Filho, preste atenção!'.\n");
        sleep(3);
        write(fd[1], pa_msg,strlen(pa_msg));//strlen(pa_msg) é o comprimento da mensagem escrita pela funcao write
        bzero(pa_msg, STR_LEN);
        read(ch_fd[0],pa_msg,strlen(pa_msg));
        printf("Pai leu: '%s'\n",pa_msg);
        waitpid(-1,&status,0);// System Call waitpid aguarda qualquer processo filho terminar. Ela é bloqueante
        printf("Pai: filho terminou...\n");
    }
    return 0;
}