#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <sys/user.h>
#include <inttypes.h>
#include "profile.h"
// Global variable in the child process

void child_process() {
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
        perror("ptrace(TRACEME)");
        exit(1);
    } 
    // Stop the child so the parent can attach
    raise(SIGSTOP);
    write(STDOUT_FILENO, "cont\n", 6);
    // Infinite loop to keep the child alive
    while(1) {
        write(STDOUT_FILENO, "tick\n", 6);
        sleep(3);
    }
}

void peek() {
    pid_t child_pid = fork();
    if (child_pid == -1) {
        perror("fork");
        exit(1);
    }
    if (child_pid == 0) {
        // Child process
        child_process();
    } else {
        // Parent process
        int status;
        printf("Parent process. Child PID: %d\n", child_pid);
        // Wait for child to stop
        waitpid(child_pid, &status, 0);
        if (WIFSTOPPED(status)) {
          printf("child stopped\n");
        }
        if (WIFEXITED(status)){
            perror("child died :(");
            exit(errno);
        }
        ptrace(PTRACE_SETOPTIONS, child_pid, NULL, PTRACE_O_EXITKILL);
        // Attach to the child process
        size_t base_address = get_process_base_address(child_pid);
        InstnTable* src_map = map_functions("./bin/cricket");
        // ptrace(PTRACE_SETOPTIONS, child_pid, NULL, PTRACE_O_EXITKILL); 
        InstnNode* fn = table_get_fn(src_map, "child_process");
        size_t peek_addr = fn->instn + base_address;
        printf("\n\n");
        printf("child base address %p\n", base_address);
        printf("fn name %s\n", fn->info.source.function->name);
        printf("peek offset %p\n", (void*)fn->instn);
        printf("peek address %p\n", (void*)(peek_addr));
        long peeked = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)peek_addr, NULL);
        if (peeked == -1 && errno != 0){
            perror("peek failed");
            exit(errno);
        }
        uint32_t word = (uint32_t)peeked;
        printf("peeked data [%u]\n", peeked);
        printf("peeked data [%u]\n", word);
        printf("hex peeked data [" "%08" PRIx32 "]\n", peeked);
        printf("hex peeked data [" "%08" PRIx32 "]\n", word);

        ptrace(PTRACE_CONT, child_pid, NULL, NULL);
        // ptrace(PTRACE_DETACH, child_pid, NULL, NULL);

        // Wait for the child to stop after attach
        waitpid(child_pid, &status, 0);
        printf("all done\n");
        while (1){
            ;
        }
        printf("how did we get here?\n");
    }
}