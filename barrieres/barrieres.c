#include "stdsys.h"
#include "barrieres.h"
#include <stdio.h>
#include <stdlib.h>

void init_barrier(struct barrier *barrier, int nb)
{
	init_lock(&(barrier->__lock));
	barrier->__nb_waiting  = 0;
	barrier->__nb_expected = nb;
}

void wait_barrier(struct barrier *barrier)
{
	lock(&(barrier->__lock));
	barrier->__nb_waiting += 1;
	if(barrier->__nb_waiting == barrier->__nb_expected)
	{
		wakeup(barrier);
		barrier->__nb_waiting = 0;
	}
	else{
		sleep(barrier, &(barrier->__lock));
	}
	unlock(&(barrier->__lock));
}
