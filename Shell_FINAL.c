#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>

#define LISTEN 1
#define READ 0

void createSimpleProcess(char *comand[]);
void createProcessAndOrOR(char *comand[], char *comandIfTrue[], char *comandIfFalse[]);
void createProcessBackground(char *comand[]);
int hasPipe(int argc, char *argv[]);
int getPosition(char delimiter,int i, char **argv);
void createProcessWithPipe(char **argv, int numberOfCOmmands);
int coutCommands(char delimiter, char **argv);
int main(int argc, char *argv[])
{
    char **cmd = &argv[1];

    if (argc == 1)
    {
        perror("Please on compiler, concatenate your comands");
    }
    else if (!hasPipe(argc, argv))
    {
        createSimpleProcess(cmd);
    }
    else if (hasPipe(argc, argv))
    {
        int qtdPipe = coutCommands('|', argv);
        createProcessWithPipe(cmd, qtdPipe);
    }
}

void createSimpleProcess(char *comand[])
{
    pid_t pid1;
    int fd[2]; // define array for pipe

    pipe(fd);      // transform fd in pipe
    pid1 = fork(); // create new process

    if (pid1 == 0)
    {                                 // if is son
        close(fd[LISTEN]);            // this close for listen
        dup2(fd[READ], STDIN_FILENO); // create an copy of fd
        close(fd[READ]);              // close
        execvp(comand[0], comand);    // exec the first comand
    }
    else
    {
        // this is father , he close all fd and wait the sons exec
        close(fd[READ]);
        close(fd[LISTEN]);
        waitpid(-1, NULL, 0);
    }
}

void createProcessAndOrOR(char *comand[], char *comandIfTrue[], char *comandIfFalse[])
{
    pid_t pid1;
    int fd[2]; // define array for pipe

    pipe(fd);      // transform fd in pipe
    pid1 = fork(); // create new process

    if (pid1 == 0)
    {                                 // if is son
        close(fd[LISTEN]);            // this close for listen
        dup2(fd[READ], STDIN_FILENO); // create an copy of fd
        close(fd[READ]);              // close
        execvp(comand[0], comand);    // exec the first comand
    }
    else
    {
        int status;
        wait(&status); /*you made a exit call in child you 
                           need to wait on exit status of child*/
        if (WIFEXITED(status))
        {
            createSimpleProcess(comandIfTrue);
            printf("child exited with = %d\n", WEXITSTATUS(status));
        }
        else
        {
            createSimpleProcess(comandIfFalse);
            printf("child exited with = %d\n", WEXITSTATUS(status));
        }
        //you should see the errno here
        // this is father , he close all fd and wait the sons exec
        close(fd[READ]);
        close(fd[LISTEN]);
        waitpid(-1, NULL, 0);
    }
}

void createProcessBackground(char *comand[])
{
    pid_t pid;
    int fd[2]; // define array for pipe
    close(fd[1]);
    if ((pid = fork()) == 0)
    {
        close(fd[LISTEN]);            // this close for listen
        dup2(fd[READ], STDIN_FILENO); // create an copy of fd
        close(fd[READ]);              // close
        execvp(comand[0], comand);    // exec the first comand
    }
}

int hasPipe(int argc, char *argv[])
{
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {
            return 1;
        }
    }
    return 0;
}

int coutCommands(char delimiter, char **argv)
{
    int comands = 0, i = 0;
    while (argv[i])
    {
        if (*argv[i] == delimiter)
        {
            comands++;
        }
        i++;
    }
    return comands;
}

int getPosition(char delimiter,int i,  char **argv)
{
    while (argv[i])
    {
        if (*argv[i] == delimiter)
            return i;
        i++;
    }
    return -1;
}

void createProcessWithPipe(char **argv, int numberOfCOmmands)
{
    int fd[2], i = 0, n; // fd[2] representa a criação de dois canais de comunicação, 1 para escrita, 0 para leitura de dados
    int aux = STDIN_FILENO;

    for (int j = 0; j <= numberOfCOmmands; j++) {
        // formata o comando atual de acordo com a posicao do pipe retornada
        n = getPosition('|', i, argv);
        char **cmd = &argv[i]; //copia o comando referente a posição na matriz de argumento
        if (n != -1) {
            cmd[n - i] = NULL;
        }

        if (pipe(fd) < 0)
        {
            perror("pipe");
        }


        pid_t pid = fork(); // cria um processo pid com seu pid
        if (pid == 0)
        {                            // pid executa comando
            close(fd[0]);            //fecha o canal para leitura
            dup2(aux, STDIN_FILENO); //duplica pipe sobre entrada padrao

            if (j < numberOfCOmmands) {
                dup2(fd[1], STDOUT_FILENO); // duplica saida padrao do pid para escrita do pipe
            }
            execvp(cmd[0], cmd); //fornece um vetor de ponteiros representando a lista de argumentos para o processo
        }
        else 
        { // pai
            aux = fd[0];
            close(fd[1]); // pai nao vai escrever
            waitpid(pid, NULL, 0);
        }
        

        i = n + 1;
    }

}