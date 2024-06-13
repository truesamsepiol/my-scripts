#include <time.h>
#include <sys/time.h>
#include <stdio.h>


#define MICROSECONDE 10000000

void test_time() {
   struct timeval start_t, end_t;
   long int sec, microsec, millisec;

   gettimeofday(&start_t, NULL);

   usleep(MICROSECONDE);

   gettimeofday(&end_t, NULL);

   sec = end_t.tv_sec - start_t.tv_sec ;
   printf("time %ld second\n", sec);

   microsec = end_t.tv_usec - start_t.tv_usec;
   printf("time %ld micro\n", microsec);

   millisec = (long int)(sec * 1000 + microsec/1000);
   printf("time %ld millisecondes\n", millisec);
}

int main(){

	printf("\n time test\n");
	test_time();
	printf("En time test\n");

	return 0;
}
