#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#define BUFSIZE 512

// void
// kmp(char s[], int pi[], int length) {
//   // assert(len(pi) <= length && len(s) <= length)
//   pi[0] = 0;
//   for(int i = 1; i < length; ++i) {
//     // aaa, 012; ababc 00120
//     int j = pi[i - 1];
//     while(j > 0 && s[j] != s[i])
//       j = pi[j - 1];
//     if(s[i] == s[j]) pi[i] = j + 1;
//     else pi[i] = 0;
//   }
// }

// int
// isContained(char s[], int pi[], int s_len, char t[], int t_len) {
//   /* if s is contained in t, that is if s is substring of t
//    * do this after pi[] is preprocessed in kmp()
//    * kmp(s, a, length);
//    * isContained(s, a, length, t, sizeof(t));
//   */
//   int pref = 0;
//   for(int i = 0; i < t_len; ++i) {
//     // ababc#cdababcdee
//     int j = pref;
//     while(j > 0 && s[j] != s[i])
//       j = pi[j - 1]
//     if(s[i] == s[j]) pref = j + 1;
//     else pref = 0;

//     if(pref == s_len) return 1; // true
//   }

//   return 0; // false
// }

void
find(char buf[], char str[], char *p)
{
  int fd;
  struct dirent de;
  struct stat st;

  if((fd = open(buf, 0)) < 0){
    fprintf(2, "find: cannot open %s\n", buf);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", buf);
    close(fd);
    return;
  }

  switch(st.type){
  case T_DEVICE:
  case T_FILE:
    if(strcmp(p, str) == 0) printf("%s\n", buf);
    break;

  case T_DIR:
    if(strlen(buf) + 1 + DIRSIZ + 1 > BUFSIZE){
      printf("find: path too long\n");
      break;
    }
    p = buf+strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(stat(buf, &st) < 0){
        printf("find: cannot stat %s\n", buf);
        continue;
      }
      if(p[0] != '.') find(buf, str, p);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  if(argc < 3) {
    fprintf(2, "Usage: find <path> <string>");
    exit(1);
  }

  char buf[BUFSIZE];
  strcpy(buf, argv[1]);
  find(buf, argv[2], buf);

  exit(0);
}
