#ifndef H_TRACEE
#define H_TRACEE 

#include <stdlib.h>
#include <sys/types.h>
#include <sys/user.h>
#include <linux/ptrace.h>

#define BREAKPOINT 0xd4200000  

typedef struct Tracee Tracee;
typedef long instruction;

instruction enable_breakpoint(pid_t, size_t);
void disable_breakpoint(pid_t, size_t, instruction);
size_t get_process_base_address(pid_t pid);
unsigned long long get_pc(pid_t);

#endif