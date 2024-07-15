syscalls: 
	cc c_system.c -o c_system -lseccomp

signals: 
	cc c_signals.c -o c_signals
	go build signals.go
	go build signals_child.go

time: 
	cc -g c_timecalls.c -o c_timecalls -lseccomp

funky: 
	cc c_funky.c -o c_funky -lseccomp

src_map: 
	cc src_map.c -o src_map -ldw 

clean:
	rm -f c_system c_signals signals signals_child c_timecalls c_funky src_map

all: syscalls signals time funky src_map

.PHONY: all clean
