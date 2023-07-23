#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/syscall.h"
#include "user/user.h"


void printsysinfo(){

  struct sysinfoStruct info;
  sysinfo(&info);
  printf("Free bytes: %d\n",info.freemem);
  printf("Number of processes: %d\n",info.nproc);
  
}

int
main(int argc, char *argv[])
{
  
  printsysinfo();
  /*void *p=malloc(1000*1000);
  printsysinfo();
  free(p);
  printsysinfo();
  */

  exit(0);
}
