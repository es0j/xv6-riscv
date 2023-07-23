#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"

char buf[512];

int
main(int argc, char *argv[])
{
  //int fd, i;

  if(argc < 3){
    fprintf(1,"trace: <traceMask> process [args]\n");
    exit(0);
  }

  char **argv_child = argv+2;

  trace(atoi(argv[1]));
  exec(argv_child[0],argv_child);

  exit(0);
}
