#include "stdsys.h"

int syscall(unsigned int value, unsigned int arg1, unsigned int arg2,
            unsigned int arg3, unsigned int arg4) {
  (void)value;
  (void)arg1;
  (void)arg2;
  (void)arg3;
  (void)arg4;
  __asm__ volatile("\tjump 0x0");

  return 0;
}

int socket_read(unsigned int d, void *buf, unsigned int len) {
  return syscall(d, (int)buf, len, 0, 11);
}

int socket_write(unsigned int d, void *buf, unsigned int len) {
  return syscall(d, (int)buf, len, 0, 12);
}

int socket_open(unsigned char penpal_role) {
  return syscall(penpal_role, 0, 0, 0, 10);
}

int socket_close(unsigned char sd) { return syscall(sd, 0, 0, 0, 13); }

int get_jid(void) { return syscall(0, 0, 0, 0, 6); }

int init_lock(struct lock *l) { return syscall((unsigned int)l, 0, 0, 0, 14); }

int lock(struct lock *l) { return syscall((unsigned int)l, 0, 0, 0, 15); }

int unlock(struct lock *l) { return syscall((unsigned int)l, 0, 0, 0, 16); }

int sleep(void *sync_addr, struct lock *l) {
  return syscall((unsigned int)sync_addr, (unsigned int)l, 0, 0, 17);
}

int wakeup(void *sync_addr) {
  return syscall((unsigned int)sync_addr, 0, 0, 0, 18);
}

int create_job(void (*fun)(void)) {
  return syscall((unsigned int)fun, 0, 0, 0, 20);
}
int join_job(void) { return syscall(0, 0, 0, 0, 21); }

int yield() { return syscall(0, 0, 0, 0, 1); }

int sbrk(int n) { return syscall(n, 0, 0, 0, 2); }

int get_pid(void) { return syscall(0, 0, 0, 0, 3); }

int get_role(void) { return syscall(0, 0, 0, 0, 4); }

int get_appid(void) { return syscall(0, 0, 0, 0, 5); }

int read_inode(unsigned int inode, void *buf, unsigned int size,
               unsigned int offset) {
  return syscall( (unsigned int)inode, (unsigned int)buf, (unsigned int)size,
                 (unsigned int)offset,22);
}

int write_inode(unsigned int inode, void *buf, unsigned int size,
                unsigned int offset) {
  return syscall((unsigned int)inode, (unsigned int)buf, (unsigned int)size,
                 (unsigned int)offset,23);
}