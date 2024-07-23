#ifndef H_BREAKPOINT
#define H_BREAKPOINT

#include "src_map/src_map.h"
#include <sys/types.h>

#define BREAKPOINT 0x00000000d4200000
#define SPLIT_WORD 0xffffffff00000000

unsigned long ptraceGetPc(pid_t);
size_t ptraceGetBaseAddress(pid_t);
void ptraceSetBreakpoints(pid_t, SrcMap*, size_t base_address);
bool ptraceContBreakpoint(pid_t, const SourceInfo*, const size_t pc);

#endif
