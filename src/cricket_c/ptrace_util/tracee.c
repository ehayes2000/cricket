#include <stdio.h>
#include <elf.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <linux/ptrace.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include "tracee.h"

instruction enable_breakpoint(pid_t child, size_t addr){
  //ptrace(enum op, pid_t, *addr, *data)
  printf("brk at %p\n", addr);
  long instn = ptrace(PTRACE_PEEKTEXT, child, (void*)addr, NULL);
  // long brk_instn = (instn & ~0x) | 0xCC; // TODO different
  // uint32_t breakpoint_instruction = 0xD4200000;
  uint32_t breakpoint_instruction = 0x00802491;
  ptrace(PTRACE_POKETEXT, child, (void*)addr, (void*)breakpoint_instruction);
  return instn;
}

void disable_breakpoint(pid_t child, size_t addr, long original){ 
  ptrace(PTRACE_POKETEXT, child, (void*)addr, (void*)original);
}

size_t get_process_base_address(pid_t pid) {
    char filename[32];
    sprintf(filename, "/proc/%d/maps", pid);
    
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Failed to open memory map");
        exit(1);
    }
    size_t base_address;
    if (fscanf(fp, "%lx", &base_address) != 1) {
        fprintf(stderr, "Failed to read base address\n");
        exit(1);
    }
    fclose(fp);
    return base_address;
}

unsigned long long get_pc(pid_t pid) {
  struct user_regs_struct regs;  
  struct iovec iov;
  iov.iov_base = &regs;
  iov.iov_len = sizeof(regs);
  ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
  return regs.pc;
}

void get_cpu_time(struct rusage *usage){
  if (getrusage(RUSAGE_CHILDREN, usage) == -1){
    fprintf(stderr, "error collecting usage %s\n", strerror(errno));
    exit(errno);
  }
}
