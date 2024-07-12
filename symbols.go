package main

import (
	"debug/elf"
	"debug/gosym"
	"debug/macho"
)

func GetSymTableElf(binFile string) (*gosym.Table, error) {
	bin, err := elf.Open(binFile)
	if err != nil {
		return nil, err
	}
	defer bin.Close()
	lnTabData, err := bin.Section(".gopclntab").Data()
	if err != nil {
		return nil, err
	}
	textAd := bin.Section(".text").Addr
	lnTable := gosym.NewLineTable(lnTabData, textAd)

	symTabData, err := bin.Section(".gosymtab").Data()
	if err != nil {
		return nil, err
	}
	return gosym.NewTable(symTabData, lnTable)
}

func GetSymTableMacho(binFile string) (*gosym.Table, error) {
	bin, err := macho.Open(binFile)
	if err != nil {
		return nil, err
	}
	defer bin.Close()
	lnTableData, err := bin.Section("__gopclntab").Data()
	if err != nil {
		return nil, err
	}
	symTableData, err := bin.Section("__gosymtab").Data()
	if err != nil {
		return nil, err
	}
	text := bin.Section("__text").Addr
	lnTable := gosym.NewLineTable(lnTableData, text)
	return gosym.NewTable(symTableData, lnTable)
}
