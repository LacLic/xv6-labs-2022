#include "kernel/types.h"
#include "user/user.h"

int listen(int fd, char *buf, char size) {
  while(1) {
    int n = read(fd, buf, size); // Read n bytes into buf; returns number read; or 0 if end of file.
    if(n > 0) return n;
  }
  
  return -1;
}

int
main(int argc, char *argv[])
{
  int p[2]; // pipe
  if(pipe(p) < 0) { // Create a pipe, put read/write file descriptors in p[0] and p[1].
    fprintf(2, "pipe() failed\n");
    exit(1);
  }

  int son_pid = fork();
  int pid = getpid();
  char buf[64];

  if(son_pid > 0){
    write(p[1], "ping", 4); // Write n bytes from buf to file descriptor fd; returns n.
    listen(p[0], buf, sizeof(buf));
    fprintf(1, "%d: received %s\n", pid, buf);
  }else{
    listen(p[0], buf, sizeof(buf));
    fprintf(1, "%d: received %s\n", pid, buf);
    write(p[1], "pong", 4);
  }

  exit(0);
}
