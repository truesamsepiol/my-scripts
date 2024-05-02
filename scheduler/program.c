#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){
	printf("\n\nJe suis le programme %s avec %d parametres\n", argv[0], argc);
	printf("	mes parametres --> ");
	for(int id = 1; id < argc; id++)
		printf("%s ", argv[id]);
	printf("\n\n\n");

	return 0;
}
