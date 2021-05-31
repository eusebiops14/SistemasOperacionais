#include<stdio.h>
#include<stdlib.h>


//argCount = quantidade de argumentos
//*argv = vetor de argumentos

int main(int argc, char *argv[]){

	int i = 0;

	for(i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	printf("/n");
	
return 0;
}
