syscalls: 
	cc c_system.c -o c_system -lseccomp

signals: 
	cc c_signals.c -o c_signals
	go build signals.go
	go build signals_child.go

time: 
	cc c_timecalls.c -o c_timecalls -lseccomp

clean:
	rm -f c_system c_signals signals signals_child c_timecalls

all: syscalls signals time

.PHONY: all clean
