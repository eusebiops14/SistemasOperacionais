#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>

#define LISTEN 1
#define READ 0

void createProcessAndOrOR(char *comand[], char *comandIfTrue[], char *comandIfFalse[]);
void createSimpleProcess(char *comand[]);

int main(int argc, char *argv[])
{
    // char allString[10][100][100];

    char *cmd1[] = {"ping", "-c10", "www.unifesp.br", NULL};
    char *cmd2[] = {"echo", "Servidor disponivel", NULL};
    char *cmd3[] = {"echo", "Servidor indisponivel", NULL};

    createProcessAndOrOR(cmd1, cmd2, cmd3);
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
