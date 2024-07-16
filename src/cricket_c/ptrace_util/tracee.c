#include <stdio.h>
#include <elf.h>
#include <sys/uio.h>
#include <sys/resource.h>
#include <linux/ptrace.h>
#include <sys/ptrace.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "tracee.h"

long enable_breakpoint(pid_t child, size_t addr){
  //ptrace(enum op, pid_t, *addr, *data)
  long instn = ptrace(PTRACE_PEEKTEXT, child, (void*)addr, NULL);
  // long brk_instn = (instn & ~0x) | 0xCC; // TODO different
  ptrace(PTRACE_POKETEXT, child, (void*)addr, (void*)BREAKPOINT);
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

struct rusage get_cpu_time(pid_t pid){
  struct rusage usage;
  if (ptrace(PTRACE_GETRUSAGE, pid, NULL, &usage) == -1){ 
    sprintf(stderr, "unable to time pid %s\n", strerror(errno));
    exit(errno);
  }
  return usage;
}
