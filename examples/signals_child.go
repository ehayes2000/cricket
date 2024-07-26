//go:build ignore
// +build ignore

package main

import (
	"fmt"
)

func sayHi() {
	fmt.Printf("Hi hello \n")
	fmt.Printf("Hi hello \n")
	fmt.Printf("Hi hello \n")
	fmt.Printf("Hi hello \n")
}

func main() {
	sayHi()
	fmt.Printf("Child started\n")
	fmt.Println("more stuff from the child A")
	fmt.Println("more stuff from the child B")
	fmt.Println("more stuff from the child C")
	fmt.Println("more stuff from the child D")
}
