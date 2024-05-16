#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 1000

int main(){
	unsigned int vector[MAX_SIZE];

	for(int id = 0; id < MAX_SIZE; id++)
		vector[id] = 2*id;

	unsigned int somme = 0;
	for(int id = 0; id < MAX_SIZE; id++)
		somme += vector[id];

	printf("somme ==  %d\n", somme);

	return 0;
}
