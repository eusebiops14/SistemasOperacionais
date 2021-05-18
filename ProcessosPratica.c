#include<stdio.h>
#include<sys/types.h>
#include<unistd.h> //Unix Standard
#include<errno.h>

/*Fork cria um novo processo duplicando o processo atual. O novo processo recebe o nome de processo filho enquanto o processo que chamou o processo filho
recebe o nome de processo pai.
-Processo pai e processo filho estão instanciados em espaços de memoria separados e ambos os espaços tem o mesmo conteudo.
-PID do pai e PID do filho são diferentes 
-Retorno da função fork(): em caso de sucesso, o PID do processo filho é retornado para o processo pai e zero é retornado para o processo filho. Em caso de erro, -1 é retornado para o
processo pai, não há criacão de um processo filho e a variavel errno (error number) é configurada com valor apropriado.
  */


int main(int argc, char **argv) {
    //pid_t pid; //pid_t é um tipo process ID 
    
    pid_t pid = fork();
    if (pid == 0) { //processo filho
        while (1) {
            printf("Filho pid=%ld, PID=%ld\n",(long)pid,(long)getpid());
            sleep(1); //dorme 1 segundo
        }
    }else if (pid > 0) { //processo pai
        while(1) {
            printf("Pai pid=%ld, PID=%ld\n",(long)pid,(long)getpid());
            sleep(1);
        }
    }else { //erro
        perror("fork()");
        return -1;
    }
    return 0;
    }
