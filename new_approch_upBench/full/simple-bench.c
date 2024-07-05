#include <mram.h>
#include <stdio.h>

void loop(void);

__mram_noinit uint8_t nb_loop[8];

int main(void) {
  while (nb_loop[0]--) {
    loop();
  }

  return 0;
}

void loop(void) {   //take 1 seconde to be done
  int a = 0;
  for (int i = 0; i < 10000; i++)
    for (int j = 0; j < 287; j++)
      a++;
}
