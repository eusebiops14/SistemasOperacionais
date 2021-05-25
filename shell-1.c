//SHELL SEM O PIPE (SEM ENCADEAMENTO DE COMANDO FUNCIONANDO)

#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>

int main(int argc, char **argv) {
    pid_t pid;
    if (argc == 1) {
        printf("Adicione mais comandos\n");
        return 0;
    }
    pid = fork();
    if (pid == 0) { //processo filho- executa o comando
        char **cmd;
        cmd = &argv[1];
        execvp(cmd[0],cmd);
    }else if (pid > 0) { //processo pai - aguarda o termino do filho
        int status;
        waitpid(-1,&status,0);
    }else { //erro
        perror("fork()");
        return -1;
    }
    //printf("Rodou\n");
    return 0;
}