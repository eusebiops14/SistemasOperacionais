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

void createSimpleProcess(char *comand[]);
int hasPipe(int argc, char *argv[]);
int coutCommands(char delimiter, char **argv);
void createProcessWithPipe(char **argv, int numberOfCommands);
int getPosition(char delimiter, int i, char **argv);
void createProcessBackground(char *comand[]);
int hasAmpersand(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    char **cmd = &argv[1];

    if (argc == 1)
    {
        perror("Please on compiler, concatenate your comands");
    }
    else if (!hasPipe(argc, argv) && !hasAmpersand(argc, argv) && !hasSemicolon(argc, argv))
    {
        createSimpleProcess(cmd);
    }
    else if (hasPipe(argc, argv))
    {
        int qtdPipe = coutCommands('|', argv);
        createProcessWithPipe(cmd, qtdPipe);
    }
    else if (hasAmpersand(argc, argv))
    {
        createProcessBackground(cmd);
    }
    else if (hasSemicolon(argc, argv))
    {
    }
}

void createSimpleProcess(char *comand[])
{
    pid_t pid1;
    int fd[2];

    pipe(fd);
    pid1 = fork();

    if (pid1 == 0)
    {
        close(fd[LISTEN]);
        dup2(fd[READ], STDIN_FILENO);
        close(fd[READ]);
        execvp(comand[0], comand);
    }
    else
    {

        close(fd[READ]);
        close(fd[LISTEN]);
        waitpid(-1, NULL, 0);
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

int hasSemicolon(int argc, char *argv[])
{
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], ";") == 0)
        {
            return 1;
        }
    }
    return 0;
}

int hasAmpersand(int argc, char *argv[])
{
    for (int i = 0; i < argc - 1; i++)
    {
        if (strcmp(argv[i], "&") == 0)
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

void createProcessWithPipe(char **argv, int numberOfCommands)
{
    int i = 0, position;
    int communication = STDIN_FILENO;

    for (int j = 0; j <= numberOfCommands; j++)
    {
        position = getPosition('|', i, argv);
        char **cmd = &argv[i];
        if (position != -1)
        {
            cmd[position - i] = NULL;
        }

        int fd[2];
        pipe(fd);

        pid_t pid = fork();
        if (pid == 0)
        {
            close(fd[0]);
            dup2(communication, STDIN_FILENO);

            if (j < numberOfCommands)
            {
                dup2(fd[1], STDOUT_FILENO);
            }
            execvp(cmd[0], cmd);
        }
        else
        {
            communication = fd[0];
            close(fd[1]);
            waitpid(pid, NULL, 0);
        }

        i = position + 1;
    }
}

int getPosition(char delimiter, int i, char **argv)
{
    while (argv[i])
    {
        if (*argv[i] == delimiter)
            return i;
        i++;
    }
    return -1;
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

void anyCommands(char **argv, int numberOfCommands)
{
    int i = 0, position;
    int communication = STDIN_FILENO;

    for (int j = 0; j <= numberOfCommands; j++)
    {
        position = getPosition('|', i, argv);
        char **cmd = &argv[i];
        if (position != -1)
        {
            cmd[position - i] = NULL;
        }

        createSimpleProcess(cmd);
        i = position + 1;

    }
}