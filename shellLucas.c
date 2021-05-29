#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAXTAM 100

void verif(char **teste, int size, int cPipe[], int checkEnd){
  int i=1;
  printf("\n\ncalled?\n\n");
  //printf("size:%d  check:%d", size, checkPipe);
  /*while(i<size && i!=checkPipe){
    if((i<checkPipe) || (i>checkPipe)){
        printf("i:%d // %s \n",i, teste[i]);
        //exe(...)
        //chama execução passando os parâmetros que estão separados...
    }
    i++;
  }*/printf("\n================\n");
    printf("size: %d\n", size);
    printf("checkEnd: %d", checkEnd);
    printf("\n================\n");

    for (i=1; i<size; i++){
      printf("(%d<%d || %d>%d)\n", i,cPipe[i],i,cPipe[i]);
      if(((i<cPipe[i]) || (i>cPipe[i])) && (i!=checkEnd)){
        printf("i:%d // %s \n",i, teste[i]);
      } else if (i==checkEnd){
        printf("outro comando");
        printf("%s \n", teste[i]);
      }
      printf("\n----------------\n");
    }
  printf("\n\ncalled!");
}



//   ./main ls -la '|' grep "teste" '&' ls -la '|' grep "teste2"

int main(int argc, char **argv){
  int i ,j , pipe[100], cPipe = 1, checkPipe = 0, checkEnd = 0;
  char *sub, **cmd;

  for (i=1; i<argc; i++){
      if (strcmp(argv[i], "|") == 0) { //strcmp = string compare
        pipe[cPipe] = i;
        cPipe++;
        checkPipe = i;
      }
      if (strcmp(argv[i], "&&") == 0) {
        printf("&&: %d ", i);
      }
      if (strcmp(argv[i], "&") == 0) { //quando encerra
        checkEnd = i;
      }
      printf("%s\n", argv[i]);
    }

    verif(argv, i, pipe, checkEnd);
    printf("%d", i);
  return 0;
}