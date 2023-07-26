#include "kernel/types.h"
#include "user/user.h"

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
    read(p[0], buf, sizeof(buf));
    fprintf(1, "%d: received %s\n", pid, buf);
  }else{
    read(p[0], buf, sizeof(buf));
    fprintf(1, "%d: received %s\n", pid, buf);
    write(p[1], "pong", 4);
  }

  close(p[0]);
  close(p[1]);

  exit(0);
}
