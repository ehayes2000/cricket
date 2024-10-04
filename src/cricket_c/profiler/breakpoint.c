#include "breakpoint.h"
#include <stdio.h>
#include <elf.h>
#include <unistd.h>
#include <sys/uio.h>
#include <sys/user.h>
#include <sys/resource.h>
#include <sys/user.h>
#include <sys/ptrace.h>
#include <sys/signal.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>


unsigned long ptraceGetPc(pid_t pid){
  struct user_regs_struct regs;  
  struct iovec iov;
  iov.iov_base = &regs;
  iov.iov_len = sizeof(regs);
  ptrace(PTRACE_GETREGSET, pid, NT_PRSTATUS, &iov);
  return regs.pc;
}

size_t ptraceGetBaseAddress(pid_t pid){
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

void ptraceSetBreakpoints(pid_t pid, SrcMap* t, size_t base_address){
    long brk_instn;
    for (InstnNode* i = srcMapIter(t); i != NULL; i = i->next){
        long aft = ptrace(PTRACE_PEEKTEXT, pid, i->offset + base_address + 4, 0);
        i->source.instn = ptrace(PTRACE_PEEKTEXT, pid, i->offset + base_address, 0);
        brk_instn = (i->source.instn & SPLIT_WORD) | BREAKPOINT;
        ptrace(PTRACE_POKETEXT, pid, i->offset + base_address, brk_instn);
        long t = ptrace(PTRACE_PEEKTEXT, pid, i->offset + base_address, NULL);
        long aft2 = ptrace(PTRACE_PEEKTEXT, pid, i->offset + base_address + 4, NULL);
    }
}

bool ptraceContBreakpoint(pid_t pid, const SourceInfo* i, const size_t pc){
    int status;
    long brk_instn = (i->instn & SPLIT_WORD) | BREAKPOINT;
    if (ptrace(PTRACE_POKETEXT, pid, pc, i->instn) == -1){
        return false;
    }
    if (ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL) == -1){
        return false;
    }
    if (waitpid(pid, &status, 0) == -1){
        return false;
    }
    if (!WIFSTOPPED(status) || !(WSTOPSIG(status) == SIGTRAP)){
        fprintf(stderr, "unexpected wait condition in continue after breakpoint\n");
        return false;
    }
    if (ptrace(PTRACE_POKETEXT, pid, pc, brk_instn) == -1){
        return false;
    }
    if (ptrace(PTRACE_CONT, pid, NULL, NULL) == -1) {
        return false;
    }
    return true;
}