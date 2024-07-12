package main

import "fmt"

func dtpnk() int {
	dt := 0b0101010100
	pnk := 0b0011001100
	return dt ^ pnk
}

func main() {
	fmt.Printf("\nhello world\n")
}
