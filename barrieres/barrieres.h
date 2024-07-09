struct barrier {
  struct lock __lock; 
  unsigned int __nb_waiting; 
  unsigned int __nb_expected;
};

int init_barrier(struct barrier *,int nb);

int wait_barrier(struct barrier *);
