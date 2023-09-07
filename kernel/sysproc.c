#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}


uint64 walk_next_pt(uint64 pt,uint64 index){
  //l1 index
  uint64 ptel1 = (uint64)PTE2PA(((pagetable_t)pt)[index]);
  if(ptel1==0){
    printf("Invalid address\n");
    return 0;
  }
  //printf("pte %p next: %p\n",((pagetable_t)pt)[index],ptel1);
  return ptel1;
}

uint64 *getpte(uint64 va){

  uint64 next;
  uint64 *res;
  next = (uint64)myproc()->pagetable;
  //l2 index
  next = walk_next_pt(
            next,
            ((va >> (12+9+9)) & 0x1ffU)  );

  next = walk_next_pt(
          next,
          ((va >> (12+9)) & 0x1ffU)  );
  res = &((pagetable_t)next)[((va >> (12)) & 0x1ffU)];

  return res;
}

/*
$ pgtbltest
ugetpid_test starting
Pid1 : 4
ugetpid_test: OK
pgaccess_test starting
base: 0x0000000000004010 pages:32 result: 0x0000000000003f9c
mask 0x0000000000000001
base: 0x0000000000004010 pages:32 result: 0x0000000000003f9c
mask 0x0000000040000006
pgaccess_test: OK
pgtbltest: all tests succeeded
$
*/

uint64 sys_pgaccess(void){

  uint64 base;
  uint64 result;
  uint64 mask=0;
  uint64 *pte;
  int pages;

  argaddr(0,&base);
  argint(1,&pages);
  argaddr(2,&result);

  printf("base: %p pages:%d result: %p\n",base,pages,result);

  for(int i=0;i<pages;i++){
    pte= getpte(base+i*PGSIZE);
    if(*pte & PTE_A){
      mask|=(1UL << i);
      //clear
      *pte = (*pte & ~PTE_A );
    }
    //printf("pte %p\n",pte);
  }
  printf("mask %p\n",mask);
  copyout(myproc()->pagetable,result,(char*)&mask,8);

  return 0;
}