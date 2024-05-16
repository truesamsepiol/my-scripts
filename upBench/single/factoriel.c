#include <stdio.h>
#include <stdlib.h>

#define MAX_ 8

int factoriel(int n){
	if(n == 0 || n == 1)
		return 1;
	return n*factoriel(n-1);
}

int main(){
	for(int id = 0; id < MAX_; id++)
		printf("fac(%d) = %d\n", id, factoriel(id));

	return 0;
}
