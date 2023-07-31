#include "kernel/types.h"
#include "user/user.h"

/*
p = get a number from left neighbor
print p
loop:
  n = get a number from left neighbor
  if (p does not divide n)
    send n to right neighbor
*/

int
main(int argc, char *argv[])
{
  int left_pipe[2], right_pipe[2];

  if(pipe(right_pipe) < 0){
    fprintf(2, "pipe() failed\n");
    exit(1);
  }

  int son_pid = fork();

  if(son_pid > 0) {
    for(int i = 2; i <= 35; ++i) {
      sleep(1); // avoid allocating too much memory at one time, or you will easily get "qemu-system-riscv64: terminating on signal 15" in make grade
      // but there is nothing wrong when testing with "make qemu" and execute "primes"
      write(right_pipe[1], (char *) &i, sizeof(int));
    }
    close(right_pipe[0]);
    close(right_pipe[1]);
    // fprintf(2, "pipe closed\n");
    exit(0);
  }

  while(1) {
    left_pipe[0] = right_pipe[0];
    left_pipe[1] = right_pipe[1];
    close(left_pipe[1]); // close the write end file descripter

    if(pipe(right_pipe)) {
      fprintf(2, "pipe() failed\n");
      exit(1);
    }

    // close 
    int int_buffer[1];
    int size = read(left_pipe[0], (char *) int_buffer, sizeof(int)) / 4;

    if(size == 0) {
      exit(0);
    }

    int n = int_buffer[0];
    fprintf(1, "prime %d\n", n);

    if(n == 35) {
      printf("break point\n");
    }

    son_pid = fork();
    if(son_pid > 0) {
      close(right_pipe[0]);
      while(1) {
        // fprintf(2, "start to read\n");
        size = read(left_pipe[0], (char *) int_buffer, sizeof(int)) / 4;
        // fprintf(2, "size = %d\n", size);
        if(size == 0) break;

        if(int_buffer[0] % n == 0) {
          // drop
        }else {
          write(right_pipe[1], (char *) int_buffer, sizeof(int));
        }
      }
      close(left_pipe[0]);
      close(right_pipe[1]);
      // fprintf(2, "pipe closed\n");
      break;
    }else {
      close(left_pipe[0]);
    }
  }

  exit(0);
}
