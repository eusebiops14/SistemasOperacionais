#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <dirent.h>
//#include <pwd.h>
#include <locale.h>

#define SPACE ' '
#define PIPE '|'
#define MAIOR '>'
#define MENOR '<'
#define MAIORDUPLO '>>'
#define ASPAS '"'

typedef struct comando
{
  char **argv;
} comando;

void getCurrentDir()
{
  char cwd[PATH_MAX];
  if (getcwd(cwd, sizeof(cwd)) != NULL)
  {
    printf("%s$", cwd);
  }
}

int criaProcessoFilho(int entrada, int saida, struct comando *cmd, int posicao)
{
  pid_t pid;
  int i;
  if ((pid = fork()) == 0)
  {
    if (entrada != 0)
    {
      dup2(entrada, 0);
      close(entrada);
    }
    if (saida != 1)
    {
      dup2(saida, 1);
      close(saida);
    }
    if (posicao != -1)
    {
      if (strcmp(cmd->argv[posicao], ">>") == 0)
      {
        saida = open(cmd->argv[posicao + 1], O_RDWR | O_APPEND);
      }
      else if (strcmp(cmd->argv[posicao], ">") == 0)
      {
        saida = open(cmd->argv[posicao + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
      }
      else if (strcmp(cmd->argv[posicao], "<") == 0)
      {
        entrada = open(cmd->argv[posicao + 1], O_RDWR);
      }
      else
      {
        return -1;
      }
      cmd->argv[posicao] = NULL;
      cmd->argv[posicao - 1] = NULL;
      cmd->argv[posicao + 1] = NULL;

      struct comando newCmd[2];
      for (i = 0; i < posicao - 2; i++)
      {
        newCmd[0].argv[i] = cmd->argv[i];
      }
      newCmd[0].argv[posicao] = NULL;
      return execvp(newCmd[0].argv[0], (char *const *)newCmd[0].argv);
    }
    else
    {
      return execvp(cmd->argv[0], (char *const *)cmd->argv);
    }
  }
  return pid;
}

void vetorPipes(int n, struct comando *cmd, int posicao[][100])
{
  int i, j;
  pid_t pid;
  int entrada, fd[2];
  entrada = 0;
  int saida = 0;
  char *comandoAux[100];
  char **comando;
  char **comandoSaida;
  char **comandoEntrada;

  for (i = 0; i < n - 1; ++i)
  {
    pipe(fd);

    if (posicao[1][i] > 0)
    {

      entrada = open(cmd[i].argv[posicao[1][i] + 1], O_RDWR | S_IRUSR | S_IWUSR);
      if (posicao[0][i] < 0)
      {

        for (j = 0; j < posicao[1][i]; j++)
        {
          comandoAux[j] = cmd[i].argv[j];
        }

        free(cmd[i].argv);

        comandoEntrada = malloc(posicao[1][i] * sizeof(char *));

        for (j = 0; j < posicao[1][i]; j++)
        {
          comandoEntrada[j] = comandoAux[j];
        }
        comandoEntrada[posicao[1][i]] = NULL;

        posicao[0][i] -= 2;
      }
      else
      {

        for (j = 0; j < posicao[1][i]; j++)
        {
          comandoAux[j] = cmd[i].argv[j];
        }

        for (j = posicao[0][i]; j < posicao[0][i] + 2; j++)
        {
          comandoAux[j - 2] = cmd[i].argv[j];
        }

        free(cmd[i].argv);

        comandoEntrada = malloc(posicao[1][i] + 3 * sizeof(char *));

        for (j = 0; j < posicao[1][i] + 2; j++)
        {
          comandoEntrada[j] = comandoAux[j];
        }
        comandoEntrada[posicao[1][i] + 3] = NULL;
      }

      dup2(entrada, 0);
      criaProcessoFilho(entrada, fd[1], comandoEntrada, posicao[0][i]);
    }
    else
    {
      criaProcessoFilho(entrada, fd[1], cmd + i, posicao[0][i]);
    }
    close(fd[1]);
    entrada = fd[0];
  }

  if (posicao[1][i] > 0)
  {

    entrada = open(cmd[i].argv[posicao[1][i] + 1], O_RDWR | S_IRUSR | S_IWUSR);
    if (posicao[0][i] < 0)
    {

      for (j = 0; j < posicao[1][i]; j++)
      {
        comandoAux[j] = cmd[i].argv[j];
      }

      free(cmd[i].argv);

      comandoEntrada = malloc(posicao[1][i] * sizeof(char *));

      for (j = 0; j < posicao[1][i]; j++)
      {
        comandoEntrada[j] = comandoAux[j];
      }
      comandoEntrada[posicao[1][i]] = NULL;

      posicao[0][i] -= 2;
    }
    else
    {

      for (j = 0; j < posicao[1][i]; j++)
      {
        comandoAux[j] = cmd[i].argv[j];
      }

      for (j = posicao[0][i]; j < posicao[0][i] + 2; j++)
      {
        comandoAux[j - 2] = cmd[i].argv[j];
      }

      free(cmd[i].argv);

      comandoEntrada = malloc(posicao[1][i] + 3 * sizeof(char *));

      for (j = 0; j < posicao[1][i] + 2; j++)
      {
        comandoEntrada[j] = comandoAux[j];
      }
      comandoEntrada[posicao[1][i] + 3] = NULL;
    }

    dup2(entrada, 0);
  }
  if (entrada != 0)
  {
    dup2(entrada, 0);
  }

  if (posicao[0][i] > 0)
  {

    if (posicao[1][i] > 0)
    {
      comandoSaida = comandoEntrada;
    }
    else
    {
      comandoSaida = cmd[i].argv;
    }

    if (comandoSaida[posicao[0][i]][1] == MAIOR)
    {
      saida = open(comandoSaida[posicao[0][i] + 1], O_RDWR | O_APPEND, O_CREAT, S_IRUSR | S_IWUSR);
    }
    else
    {
      saida = open(comandoSaida[posicao[0][i] + 1], O_RDWR | O_TRUNC, O_CREAT, S_IRUSR | S_IWUSR);
    }

    for (j = 0; j < posicao[0][i]; j++)
    {
      comandoAux[j] = comandoSaida[j];
    }

    free(comandoSaida);

    comando = malloc(posicao[0][i] * sizeof(char *));

    for (j = 0; j < posicao[0][i]; j++)
    {
      comando[j] = comandoAux[j];
    }
    comando[posicao[0][i]] = NULL;
    dup2(saida, 1);
  }

  if (posicao[0][i] > 0 || posicao[1][i] > 0)
  {
    return execvp(comando[0], (char *const *)comando);
  }
  else
  {
    if (posicao[1][i] > 0)
    {
      return execvp(comandoSaida[0], (char *const *)comandoSaida);
    }
    return execvp(cmd[i].argv[0], (char *const *)cmd[i].argv);
  }

  return 0;
}

int main()
{
  int qtdComandos = 0;
  int i, j;
  comando cmd[100];
  char inputDoUsuario[100];
  char **comandoAuxiliar;
  comandoAuxiliar = malloc(100 * sizeof(char *));
  char parametroAuxiliar[100];
  char *parametro;
  int qtdParametros = 0;
  int qtdChar = 0;
  char **comando;
  int posicaoAux[2][100];
  int qtdOperadoresMaior = 0;
  int qtdOperadoresMenor = 0;
  int acheiOperadoresMenor = 0;
  int acheiOperadoresMaior = 0;

  bool ehOperadorMaior = false;
  bool ehOperadorMenor = false;

  char *nomeUsuario;
  struct passwd *pass;
  pass = getpwuid(getuid());
  nomeUsuario = pass->pw_name;
  printf("%s:", nomeUsuario);
  getCurrentDir();
  gets(inputDoUsuario);

  inputDoUsuario[strlen(inputDoUsuario)] = '\0';

  for (i = 0; i < strlen(inputDoUsuario) + 1; i++)
  {
    if (inputDoUsuario[i] != ASPAS)
    {
      parametroAuxiliar[qtdChar] = inputDoUsuario[i];
      qtdChar++;
    }

    if (inputDoUsuario[i] == SPACE || inputDoUsuario[i] == '\0')
    {
      parametroAuxiliar[qtdChar] = '\0';
      parametro = (char *)malloc(qtdChar - 1 * sizeof(char));
      for (j = 0; j < qtdChar - 1; j++)
      {
        parametro[j] = parametroAuxiliar[j];
        parametroAuxiliar[j] = '\0';
      }
      comandoAuxiliar[qtdParametros] = malloc(strlen(parametro) * sizeof(char));

      for (j = 0; j < strlen(parametro) + 1; j++)
      {
        comandoAuxiliar[qtdParametros][j] = parametro[j];
      }
      qtdParametros++;
      qtdChar = 0;
      free(parametro);
    }

    if (inputDoUsuario[i] == PIPE || inputDoUsuario[i] == '\0')
    {
      i++;
      comando = (char **)malloc(qtdParametros + 1 * sizeof(char *));
      comandoAuxiliar[qtdParametros + 1] = NULL;
      for (j = 0; j < qtdParametros + 1; j++)
      {
        comando[j] = comandoAuxiliar[j];
        comandoAuxiliar[j] = '\0';
      }
      cmd[qtdComandos].argv = comando;

      if (ehOperadorMaior)
      {
        posicaoAux[0][qtdComandos] = acheiOperadoresMaior;
      }
      else
      {
        posicaoAux[0][qtdComandos] = -1;
      }

      if (ehOperadorMenor)
      {
        posicaoAux[1][qtdComandos] = acheiOperadoresMenor;
      }
      else
      {
        posicaoAux[1][qtdComandos] = -1;
      }

      qtdComandos++;
      qtdParametros = 0;
      qtdChar = 0;
      ehOperadorMaior = false;
      ehOperadorMenor = false;
      comando = NULL;
      free(comando);
    }

    if (inputDoUsuario[i] == MAIOR)
    {
      ehOperadorMaior = true;
      acheiOperadoresMaior = qtdParametros;
      qtdOperadoresMaior++;
    }

    if (inputDoUsuario[i] == MENOR)
    {
      ehOperadorMenor = true;
      acheiOperadoresMenor = qtdParametros;
      qtdOperadoresMenor++;
    }
  }

  vetorPipes(qtdComandos, cmd, posicaoAux);
}