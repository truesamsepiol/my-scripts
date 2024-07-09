

// int syscall(int type,int arg1, int arg2, int arg3, int arg4);

struct lock {
  unsigned int __owner;
};

#define __buffer_aligned __attribute__((aligned(8)))

/*
Give back to another process.
Be nice give back ;)
*/
int yield();

/*
Increase or decrease your memory (WRAM) of n
*/
int sbrk(int n);

int get_pid(void);
int get_role(void);
int get_appid(void);
int get_jid(void);

int socket_read(unsigned int d, void *buf, unsigned int len);
int socket_write(unsigned int d, void *buf, unsigned int len);

/*
Open a socket with the process who has penpal_role role. You must be in the same
application
*/
int socket_open(unsigned char penpal_role);

int socket_close(unsigned char sd);

int init_lock(struct lock *l);
int lock(struct lock *l);
int unlock(struct lock *l);
int sleep(void *sync_addr, struct lock *l);
int wakeup(void *sync_addr);

int create_job(void (*fun)(void));
int join_job(void);

int read_inode(unsigned int inode, void *buf, unsigned int size, unsigned int offset);
int write_inode(unsigned int inode, void *buf, unsigned int size, unsigned int offset);
