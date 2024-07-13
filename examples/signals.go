//go:build ignore
// +build ignore

package main

import (
	"fmt"
	"os"
	"os/exec"
	"syscall"
)

/*
An adaptation of signals.c


*/

var wstatus syscall.WaitStatus

func main() {
	cmd := exec.Command("./signals_child")
	cmd.Stdout = os.Stdout
	cmd.Stderr = os.Stderr
	cmd.Stdin = os.Stdin
	cmd.SysProcAttr = &syscall.SysProcAttr{
		Ptrace: true,
	}
	cmd.Start()
	// syscall.Wait4(cmd.Process.Pid, &wstatus, 0, nil)

	syscall.Wait4(cmd.Process.Pid, &wstatus, 0, nil)
	if wstatus.Stopped() {
		fmt.Printf("Child stopped on [%s] \n", wstatus.StopSignal())
	}
	if wstatus.Signaled() {
		fmt.Printf("Child signaled with [%s] \n", wstatus.Signal())
	}
	// time.Sleep(time.Second * 10)
	// syscall.Kill(cmd.Process.Pid, syscall.SIGCONT)
	syscall.PtraceCont(cmd.Process.Pid, 0)
	cmd.Process.Signal(syscall.SIGALRM)
	syscall.Wait4(cmd.Process.Pid, &wstatus, 0, nil)
	if wstatus.Stopped() {
		fmt.Printf("Child stopped on [%s] \n", wstatus.StopSignal())
	}
	if wstatus.Signaled() {
		fmt.Printf("Child signaled with [%s] \n", wstatus.Signal())
	}
	syscall.PtraceCont(cmd.Process.Pid, 0)
	syscall.Wait4(cmd.Process.Pid, &wstatus, 0, nil)
	if wstatus.Stopped() {
		fmt.Printf("Child stopped on [%s] \n", wstatus.StopSignal())
	}
	if wstatus.Signaled() {
		fmt.Printf("Child signaled with [%s] \n", wstatus.Signal())
	}
	fmt.Printf("exit\n")

}
