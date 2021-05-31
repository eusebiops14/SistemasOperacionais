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

void createProcessBackground(char *comand[]);

int main(int argc, char *argv[])
{

    char *cmd1[] = {"ping", "-c5", "www.unifesp.br", NULL};

    createProcessBackground(cmd1);
}

void createProcessBackground(char *comand[])
{
    pid_t pid;
    int fd[2]; // define array for pipe
    close(fd[1]);
    if ((pid = fork()) == 0)
    {
        close(fd[LISTEN]);                     // this close for listen
        dup2(fd[READ], STDIN_FILENO);          // create an copy of fd
        close(fd[READ]);                       // close
        execvp(comand[0], comand); // exec the first comand
    }
}
