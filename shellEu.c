#include  <fcntl.h>                              
#include  <stdio.h>                              
#include  <stdlib.h>                             
#include  <string.h>                             
#include  <sys/types.h>                          
#include  <sys/wait.h>                           
#include  <sys/stat.h>                           
#include  <termios.h>                            
#include  <unistd.h>                             
                                                 
#define LISTEN 1                              
#define READ 0 

void verif(char *teste[], int size, int cPipe[], int checkEnd){
  int i=1;
  
  printf("\n================\n");
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
/*
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
*/
                                                 
int main(int argc, char **argv) {      //**argv funciona                                          
    pid_t pid1;                                  
    pid_t pid2; // define an number for process
    int fd[2];   // define array for pipe                      

    if (argc == 1) {
        printf("Numero de comandos insuficientes. Insira comandos adicionais.\n");
        return 0;
    }                                             
    pipe(fd);  // transform fd in pipe
    pid1 = fork(); // create new process
                                                 
    if(pid1==0) {  // if is son                             
        char **cmd;
        cmd = &argv[1];
        //printf("%s",cmd);

        close(fd[LISTEN]);  // this close for listen
        dup2(fd[READ], STDIN_FILENO);      // create an copy of fd
        close(fd[READ]);                   // close 
        
        //execlp("grep","grep", "s" ,(char*) NULL);   // exec the first comand
        execvp(cmd[0],cmd);
    }                                            
    else {                                            
        pid2=fork();  // create new process            
                                                 
        if(pid2==0) {  // if is son                     
            close(fd[READ]);               
            dup2(fd[LISTEN], STDOUT_FILENO);  
            close(fd[LISTEN]);    //ls la            
            //execvp(cmd[1],cmd);
            execlp("ls","ls","-la",(char*) NULL); 
        }                                        

        // this is father , he close all fd and wait the sons exec                            
        close(fd[READ]);             
        close(fd[LISTEN]);                    
        waitpid(-1, NULL, 0);                   
        waitpid(-1, NULL, 0);                    
                                                 
    }                                            
} 