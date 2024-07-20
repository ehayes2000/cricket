// #include <unistd.h>
// #include <sys/types.h>
// #include <sys/ptrace.h>
// #include <inttypes.h>
// #include <signal.h>
// #include <errno.h>
// #include <string.h>
// #include "profile.h"

// void the_joker() { 
//   if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0){
//     perror("ptrace(TRACEME)");
//     exit(errno);
//   }
//   raise(SIGSTOP);
//   printf("I'm going to break open arkham and no one can stop me!\n");
//   printf("collecting cards ...\n");
//   printf("gathering cronies...\n");
//   printf("plotting route...\n");
//   printf("loading unmarked van...\n");
//   printf("driving to asylum...\n");
//   printf("melting gate with acid...\n");
//   printf("escaping with inmates...\n");
//   printf("we live in a society!!!\n");
// }

// void breakpoint(){
//   pid_t child = fork();
//   if (child == 0){
//     the_joker();
//     return;
//   } 
//   printf("joker pid %d\n", child);
//   ptrace(PTRACE_SETOPTIONS, child, NULL, PTRACE_O_EXITKILL);
//   int status;
//   waitpid(child, &status, NULL);
//   if (WIFEXITED(status)){ 
//     perror("The joker died :(");
//     exit(1);
//   }
//   if (!WIFSTOPPED(status)){
//     perror("we lost him");
//     exit(1);
//   }
//   printf("finding lair...\n");
//   size_t base_address = get_process_base_address(child);
//   printf("decyphering plans...\n");
//   InstnTable* src_map = map_functions("./bin/cricket");
//   InstnNode* jkr = table_get_fn(src_map, "the_joker");
//   long breakpt = 0xd4200000; //  000020d4

//   Interval* joker_time = calloc(1, sizeof(Interval));
//   for (InstnNode* i = table_iterate(src_map); i != NULL; i = i->next){
//     if (strcmp(i->info.source.function->name, "the_joker")){  
//       i->interval = joker_time;
//     }
//   }
// }
