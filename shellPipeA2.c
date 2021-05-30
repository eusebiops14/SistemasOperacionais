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

void parser(int argc, char *argv[], char allString[100][100]);

int main(int argc, char *argv[])
{
    char allString[10][100][100];


    parser(argc, argv, allString);

    pid_t pid1;
    pid_t pid2;
    int fd[2]; // define array for pipe

    pipe(fd);      // transform fd in pipe
    pid1 = fork(); // create new process

    if (pid1 == 0)
    {                                          // if is son
        close(fd[LISTEN]);                     // this close for listen
        dup2(fd[READ], STDIN_FILENO);          // create an copy of fd
        close(fd[READ]);                       // close
        execvp(allString[1][0], allString[1]); // exec the first comand
    }
    else
    {

        pid2 = fork(); // create new process

        if (pid2 == 0)
        { // if is son
            close(fd[READ]);
            dup2(fd[LISTEN], STDOUT_FILENO);
            close(fd[LISTEN]);
            execvp(allString[0][0], allString[0]); // exec the first comand
        }
        // this is father , he close all fd and wait the sons exec
        close(fd[READ]);
        close(fd[LISTEN]);
        waitpid(-1, NULL, 0);
    }
}

void parser(int argc, char *argv[], char allString[100][100])
{
    int numbersOfComands = 0, params = 0;
    char* p = NULL;
   printf( "%s", p);

    for (int i = 1; i < argc; i++)
    {
        printf(" %d --%s %d %d\n", i, argv[i], numbersOfComands, params);
        if (strcmp(argv[i], "|") == 0)
        {
            printf("pipe\n");
            allString[numbersOfComands][params] =  p;

            strcpy(allString[numbersOfComands][params], p); 
            printf("pipe\n");

            numbersOfComands++;
            params = 0;
        }
        else
        {
            strcpy(allString[numbersOfComands][params], argv[i]);
            params++;
            printf("%s\n", allString[numbersOfComands][params]);
        }
    }

    
    for (int i=0; i<4; i++) {
        printf("%s\n", allString[0][i]);
    }
    printf("%s\n", allString[0][2]);

}