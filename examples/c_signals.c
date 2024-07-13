#include <sys/ptrace.h>
// #include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>

/*
A simple c program that uses ptrace to interupt on exec call 
continue the child, then get exit status

lessons: 
  ptrace interupts on exec call with a SIGTRAP
  ptrace can also be set to interrupt on all 
  future syscalls with the PTRACE_SYSCALL flag
*/

void parent_process(pid_t);
void child_process(); 
const char* get_signal_name(int);
void override_default_handlers();
void handle_signal(int);

int main() { 
  printf("We do a bit of ptracing\n\n");
  pid_t child = fork(); 
  if (child == 0) { 
    child_process();
  } else { 
    parent_process(child);
  }
  return 0;
}

void parent_process(pid_t child) { 
  printf("Parent process\n");
  int status;
  while (1) { 
    printf("waiting ...\n");
    wait(&status);

    if (WIFEXITED(status)) { 
      int exit_code = WEXITSTATUS(status);
      printf("child exited with exit code %d\n", exit_code);
      return;
    }
    if (WIFSIGNALED(status)) { 
      printf("Child terminated by signal\n");
      int sig = WTERMSIG(status);
      printf("Signal: %s\n", get_signal_name(sig));
    } else if (WIFSTOPPED(status)) { 
      int sig = WSTOPSIG(status);
      printf("Child stopped by signal: %s\n", get_signal_name(sig));
    }
    // ptrace(PTRACE_SYSCALL, child, NULL, NULL);
    ptrace(PTRACE_CONT, child, NULL, NULL); 
  }
}

void child_process() {
  printf("Child started\n");
  override_default_handlers();
  ptrace(PTRACE_TRACEME, 0, NULL, NULL);
  execl("/bin/ls", "ls", NULL);
  printf("Exec failed :(\n");
} 

void handle_signal(int sig){
  // print signal recieved raise it to default handler
  printf("Signal recieved %s\n", get_signal_name(sig));
  signal(sig, SIG_DFL);
  raise(sig);
}

void override_default_handlers() { 
  for (int i = 0; i < NSIG; i ++){
    if (i != SIGKILL && i != SIGSTOP){
      signal(i, handle_signal);
    }
  }
}

const char* get_signal_name(int sig) {
    // llm generated
    switch(sig) {
        case SIGHUP:    return "SIGHUP";
        case SIGINT:    return "SIGINT";
        case SIGQUIT:   return "SIGQUIT";
        case SIGILL:    return "SIGILL";
        case SIGTRAP:   return "SIGTRAP";
        case SIGABRT:   return "SIGABRT";
        case SIGBUS:    return "SIGBUS";
        case SIGFPE:    return "SIGFPE";
        case SIGKILL:   return "SIGKILL";
        case SIGUSR1:   return "SIGUSR1";
        case SIGSEGV:   return "SIGSEGV";
        case SIGUSR2:   return "SIGUSR2";
        case SIGPIPE:   return "SIGPIPE";
        case SIGALRM:   return "SIGALRM";
        case SIGTERM:   return "SIGTERM";
        case SIGSTKFLT: return "SIGSTKFLT";
        case SIGCHLD:   return "SIGCHLD";
        case SIGCONT:   return "SIGCONT";
        case SIGSTOP:   return "SIGSTOP";
        case SIGTSTP:   return "SIGTSTP";
        case SIGTTIN:   return "SIGTTIN";
        case SIGTTOU:   return "SIGTTOU";
        default:        return "UNKNOWN";
    }
}