package main

import (
	"os"
	"os/exec"
	"syscall"
)

func MakeDebugCmd(binPath string) *exec.Cmd {
	cmd := exec.Command(binPath)
	cmd.Stderr = os.Stderr
	cmd.Stdout = os.Stdout
	cmd.Stdin = os.Stdin
	cmd.SysProcAttr = &syscall.SysProcAttr{
		Ptrace: true,
	}
	return cmd
}
