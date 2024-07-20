// #include <errno.h>
// #include <string.h>
// #include <stdio.h>
// #include <sys/time.h>
// #include <sys/resource.h>
// #include "profile.h"


// void handle_stop(InstnTable*, pid_t, size_t);
// void profile_functions(char* program){ 
//   // map offsets to source info
//   InstnTable* src_map = map_functions(program); 
//   pid_t child = fork();
//   // child
//   if (child == 0){
//     ptrace(PTRACE_TRACEME, 0, NULL, NULL);
//     execl(program, NULL);
//     fprintf(stderr, "exec failed with %s\n", strerror(errno));
//     exit(errno);
//   } 
//   // parent    
//   printf("child started with pid %lu\n", child);
//   int status, err;
//   err = waitpid(child, &status, 0);
//   if (err == -1){
//     fprintf(stderr, "waitpid failed with %s\n", strerror(errno));
//     exit(errno);
//   }
//   if (WIFEXITED(status)){
//     fprintf(stderr, "child exited\n");
//     exit(1);
//   }
//   // get base address of child
//   size_t base_address = get_process_base_address(child);
//   // set breakpoints at all functions
//   // for (InstnNode* i = table_iterate(src_map); i != NULL; i = i->next){
//   //   // i->info.word = enable_breakpoint(child, base_address + i->instn);
//   //   printf("set breakpoint\n");
//   //   printf("  fn: %s\n", i->info.source.function->name);
//   //   printf("  offset: %p\n", i->instn);
//   //   printf("  break at: %p\n\n", i->instn + base_address);
//   // }

//   InstnNode* poke = table_iterate(src_map);
//   char* npoke = poke->info.source.function->name;
//   size_t addr = poke->offset + base_address;
//   uint32_t ipoked = 0x00000091;
//   printf("poking %s \n  %p offset\n  %p addr\n", npoke, (void*)poke->offset, (void*)addr);
//   printf("double cast %s %zx %x\n", npoke, (size_t)((void*)addr), (uint32_t)((void*)(uintptr_t)ipoked));
//   // enable_breakpoint(child, base_address + table_iterate(src_map)->next->instn);
//   unsigned long peeked = ptrace(PTRACE_PEEKUSR, child, (void*)addr, NULL);
//   printf("peeked instn = %lx\n", peeked);

//   // ptrace(PTRACE_POKETEXT, child, (void*)addr, (void*)(uintptr_t)ipoked);
//   ptrace(PTRACE_DETACH, child, NULL, NULL);

//   while (1){
//   waitpid(child, &status, 0);
//   if (WIFEXITED(status)){
//     printf("child exited :) %s\n", strerror(errno));
//   }
//   if (WIFSIGNALED(status)){
//     int signal = WTERMSIG(status);
//     printf("child signalled %s\n", strsignal(signal));
//     printf("errno? %s\n", strerror(errno));
//   }
//   exit(1);
//   }


  
//   printf("RATS\n");
//   // // ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_EXITKILL | PTRACE_O_TRACESYSGOOD);    
//   // ptrace(PTRACE_CONT, child, NULL, NULL);
//   // while (1){
//   //   waitpid(child, &status, 0);
//   //   if (WIFEXITED(status)){
//   //     printf("child exited\n");
//   //     return;
//   //   } else if (WIFSTOPPED(status)){
//   //     handle_stop(src_map, child, base_address);
//   //     ptrace(PTRACE_DETACH, child, NULL, NULL);
//   //   }
//   //   // if (ptrace(PTRACE_CONT, child, NULL, NULL) == -1){
//   //   //   fprintf(stderr, "failed to restart child %s\n", strerror(errno));
//   //   //   exit(errno);
//   //   // }
//   // }
// }



// void handle_stop(InstnTable* src_map, pid_t child, size_t base_address){
//   unsigned long long pc = get_pc(child);
//   printf("%p stopped pc\n", pc);
//   size_t offset = pc - base_address;
//   SourceInfo src = table_get_info(src_map, offset);
//   if (src.kind == SENTINAL){
//     fprintf(stderr, "could not find source for %p\n", offset);
//     return;
//   }
//   struct rusage t;
//   get_cpu_time(&t);
//   printf("breakpoint triggered\n");
//   printf("  fn: %s\n", src.source.function->name);
//   printf("  loc: %s::%lu\n", src.source.function->src_file, src.source.function->line);
//   printf("  offset: %p\n", offset);
//   printf("  pc: %p\n", pc);
//   printf("  time: %lus %luμs\n\n", t.ru_stime.tv_sec, t.ru_stime.tv_usec);
//   disable_breakpoint(child, pc, src.word);
// }