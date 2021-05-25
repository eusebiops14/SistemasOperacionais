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
                                                 
int main(int argc, char* argv[]) {                                                
    pid_t pid1;                                  
    pid_t pid2; // define an number for process
    int fd[2];   // define array for pipe                      
                                                 
    pipe(fd);  // transform fd in pipe
    pid1 = fork(); // create new process
                                                 
    if(pid1==0) {  // if is son                             
        close(fd[LISTEN]);  // this close for listen
        dup2(fd[READ], STDIN_FILENO);      // create an copy of fd
        close(fd[READ]);                   // close 
        execlp("grep","grep", "s" ,(char*) NULL);   // exec the first comand
    }                                            
    else {                                            
        pid2=fork();  // create new process            
                                                 
        if(pid2==0) {  // if is son                     
            close(fd[READ]);               
            dup2(fd[LISTEN], STDOUT_FILENO);  
            close(fd[LISTEN]);                
            execlp("ls","ls","-la",(char*) NULL); 
        }                                        

        // this is father , he close all fd and wait the sons exec                            
        close(fd[READ]);             
        close(fd[LISTEN]);                    
        waitpid(-1, NULL, 0);                   
        waitpid(-1, NULL, 0);                    
                                                 
    }                                            
}         