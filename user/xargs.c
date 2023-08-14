#include "kernel/types.h"
#include "user/user.h"
#include "kernel/param.h"

#define BUFSIZ 512
#define INPUTSIZE 512
#define EOF ((char)-1)
#define getchar(buf, p1, p2) (p1 == p2 && (p2 = (p1 = buf) + read(0, buf, BUFSIZ), p1 == p2) ? EOF : *p1++)

int
main(int argc, char *argv[])
{
  char buf[BUFSIZ], *p1 = buf, *p2 = buf, str[INPUTSIZE];
  int loop = 1; // bool loop = true;
  while(loop) {
    char *p = str;
    while(1) {
      *p = getchar(buf, p1, p2);
      if(*p == '\n') {
        *p = '\0';
        break;
      }else if(*p == EOF) {
        loop = 0; // loop = false;
        *p = '\0';
        break;
      }else if (*p == ' ' && p > buf && *(p - 1) == ' ') {
        // pass
      }else {
        ++p;
        if(str - p >= INPUTSIZE) {
          fprintf(2, "xargs: input too long.\n");
          exit(1);
        }
      }
    }

    if(str == p && loop == 0) break;

    int son_pid = fork();
    if(son_pid == 0) {
      int cnt = 1;
      char *command, *xargv[MAXARG];
      if(argc > 1) command = argv[1];
      else command = "echo";
      xargv[0] = argv[0];
      for(int i = 2; i < argc; ++i) {
        xargv[cnt] = argv[i];
        ++cnt;
      }

      for(char *l = str, *r; l < p; l = r) {
        r = l;
        while(r < p && *r != ' ') {
          ++r;
        }
        xargv[cnt] = l;
        ++cnt;
        if(cnt + 1 >= MAXARG) {
          fprintf(2, "xargs: too many arguments.\n");
          exit(1);
        }
        if(r < p) {
          *r = '\0';
          ++r;
        }
      }
      xargv[cnt] = (char *)0;
      if(exec(command, xargv) < 0) {
        fprintf(2, "xargs: exec() failed.\n");
        exit(1);
      }
    }else {
      int ret;
      wait(&ret);
      if(ret != 0) {
        exit(1);
      }
    }
  }

  exit(0);
}
