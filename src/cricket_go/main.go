package main

import (
	"fmt"
	"syscall"
)

func main() {
	binPath := "targets/hello/hello"
	_, err := GetSymTableElf(binPath)
	if err != nil {
		fmt.Println("GG GO NEXT")
		panic(err)
	}
	cmd := MakeDebugCmd(binPath)
	err = cmd.Start()
	if err != nil {
		panic(err)
	}
	err = cmd.Wait()

	fmt.Printf("die \n")
	pid := cmd.Process.Pid
	fmt.Printf("pid %d, pgid:  %d\n", pid, -1)
	pgid, _ := syscall.Getpgid(pid) // why do we debug on the group id?
	fmt.Printf("pid %d, pgid:  %d\n", pid, pgid)
	err = syscall.PtraceAttach(pgid)
	if err != nil {
		panic(fmt.Sprintf("attach error %v\n", err))
	}
	err = syscall.PtraceSetOptions(pid, syscall.PTRACE_O_TRACECLONE)
	fmt.Printf("debugger says hi\n")
	syscall.PtraceCont(pid, 0)
	var status syscall.WaitStatus
	var rusage syscall.Rusage
	syscall.Wait4(-1, &status, 0, &rusage)
	if err != nil {
		fmt.Printf("waited %v\n", err)
	}
}
