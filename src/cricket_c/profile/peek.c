#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include "profile.h"
// Global variable in the child process
int global_var = 12345;

void child_process() {
    printf("Child process started. PID: %d\n", getpid());
    printf("Address of global_var: %p\n", (void*)&global_var);
    
    // Allow parent to attach
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
        perror("ptrace(TRACEME)");
        exit(1);
    } 
    printf("TRACEME\n");
    
    // Stop the child so the parent can attach
    raise(SIGSTOP);
    printf("how did we get here?\n") ;
    // Infinite loop to keep the child alive
    while(1) {
        sleep(1);
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
        if (WIFSTOPPED(status)) 
          printf("child stopped\n");
        // Attach to the child process
        if (ptrace(PTRACE_ATTACH, child_pid, NULL, NULL) < 0) {
            perror("ptrace(ATTACH)");
            exit(1);
        }
        
        // // Wait for the child to stop after attach
        // waitpid(child_pid, &status, 0);
        
        // // Get the address of global_var from the child's output
        // char command[256];
        // snprintf(command, sizeof(command), "grep 'Address of global_var:' /proc/%d/fd/1", child_pid);
        // FILE *fp = popen(command, "r");
        // char buffer[256];
        // fgets(buffer, sizeof(buffer), fp);
        // pclose(fp);
        
        // void *addr;
        // sscanf(buffer, "Address of global_var: %p", &addr);
        
        // // Read the value of global_var from the child's memory
        // long data = ptrace(PTRACE_PEEKDATA, child_pid, addr, NULL);
        
        // if (data == -1 && errno != 0) {
        //     perror("ptrace(PEEKDATA)");
        //     exit(1);
        // }
        
        // printf("Value read from child's memory: %ld\n", data);
        
        // // Detach from the child
        // if (ptrace(PTRACE_DETACH, child_pid, NULL, NULL) < 0) {
        //     perror("ptrace(DETACH)");
        //     exit(1);
        // }
        
        // printf("Parent process completed. You can now attach GDB to the child (PID: %d)\n", child_pid);
        
        // // Wait for user input before exiting
        // printf("Press Enter to exit the parent process...\n");
        // getchar();
    }
}