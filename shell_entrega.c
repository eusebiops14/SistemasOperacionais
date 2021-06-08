/* Eusébio pereira de souza - RA: 86565 - Noturno 
    Lucas ......
*/


#include <termios.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>


void ProcessFactory(char *comand[]); //cria processos
void ProcessFactoryTrueOrFalse(char *comand[], char *comandIfTrue[], char *comandIfFalse[]);
void BackgroundProcessFactory(char *comand[]);
int hasPipe(int argc, char *argv[]);
int getPosition(char delimiter,int i, char **argv);
void createProcessWithPipe(char **argv, int numberOfCOmmands);
int coutCommands(char delimiter, char **argv);

//Analise matriz de argumentos
int main(int argc, char *argv[])
{
    char **cmd = &argv[1];

    if (argc == 1)
    {
        perror("ATENÇÃO: Insira mais comandos");
    }
    else if (!hasPipe(argc, argv))
    {
        ProcessFactory(cmd);
    }
    else if (hasPipe(argc, argv))
    {
        int N_Pipe = coutCommands('|', argv);
        createProcessWithPipe(cmd, N_Pipe);
    }
}

void ProcessFactory(char *comand[])
{
    pid_t pid1;
    int fd[2]; 

    pipe(fd);      // Cria canal de comunicacao unidirecional com pipe
    pid1 = fork(); // cria um novo processo utilizando fork

    if (pid1 == 0) //Significa que processo é filho
    {                                 
        close(fd[1]);            // Fecha o canal de leitura
        dup2(fd[0], STDIN_FILENO); // cria uma copia de fd[0]- canal escrita
        close(fd[0]);              // fecha fd
        execvp(comand[0], comand);    // comando execvp usado no processo filho para executar o comando do shell
    }
    else
    {
        // Processo pai: Fecha os descritores de arquivo e espera pela execução do filho
        close(fd[0]);
        close(fd[1]);
        waitpid(-1, NULL, 0);
    }
}

void ProcessFactoryTrueOrFalse(char *comand[], char *comandIfTrue[], char *comandIfFalse[])
{
    pid_t pid1;
    int fd[2]; // define array para o pipe

    pipe(fd);      // transforma o file descriptor em pipe
    pid1 = fork(); // cria novo processo a partir da funcao fork

    if (pid1 == 0)
    {                                 // if is son
        close(fd[1]);            // this close for listen
        dup2(fd[0], STDIN_FILENO); // create an copy of fd
        close(fd[0]);              // close
        execvp(comand[0], comand);    // exec the first comand
    }
    else
    {
        int status;
        wait(&status); /*you made a exit call in child you 
                           need to wait on exit status of child*/
        if (WIFEXITED(status))
        {
            ProcessFactory(comandIfTrue);
            printf("filho terminou = %d\n", WEXITSTATUS(status));
        }
        else
        {
            ProcessFactory(comandIfFalse);
            printf("filho terminou = %d\n", WEXITSTATUS(status));
        }
        //you should see the errno here
        // this is father , he close all fd and wait the sons exec
        close(fd[0]);
        close(fd[1]);
        waitpid(-1, NULL, 0);
    }
}

void BackgroundProcessFactory(char *comand[])
{
    pid_t pid;
    int fd[2]; // cria descritor de arquivos 
    close(fd[1]);
    if ((pid = fork()) == 0)
    {
        close(fd[1]);            // fecha descritor de leitura
        dup2(fd[0], STDIN_FILENO); // duplica descritor de arquivo para escrita
        close(fd[0]);              // close
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