#include "Memory.h"
#include <iostream>
#include <cstring>
Mem_mu::Mem_mu(){
	// just for now
	memset(&m_memory, 0, sizeof(struct Gb_memory));
};

BYTE Mem_mu::read(WORD addr){
	if(addr >= 0x0000 && addr <= 0x3fff) return m_memory.rom_bank0[addr];
	if(addr >= 0x4000 && addr <= 0x7fff) return m_memory.rom_bank1n[addr];
	if(addr >= 0x8000 && addr <= 0x9fff) return m_memory.vram[addr & 0x1fff];
	if(addr >= 0xa000 && addr <= 0xbfff) return m_memory.ex_ram[addr & 0x1fff];
	if(addr >= 0xc000 && addr <= 0xcfff) return m_memory.wram_bank0[addr & 0x0fff];
	if(addr >= 0xd000 && addr <= 0xdfff) return m_memory.wram_bank1n[addr & 0x0fff];
	if(addr >= 0xe000 && addr <= 0xfdff) return read_wram_mirror(addr);
	if(addr >= 0xfea0 && addr <= 0xfeff) return 0x00;
};

BYTE Mem_mu::read_wram_mirror(WORD addr){
	WORD offset = addr - 0x2000;
	if(offset >= 0xc000 && offset <= 0xcfff)
		return m_memory.wram_bank0[offset & 0x0fff];
	else{
		return m_memory.wram_bank1n[offset & 0x0fff];
	}
}

void Mem_mu::write_wram_mirror(WORD addr, BYTE value){
	m_memory.wram_mirror[addr & 0x1fff] = value;
	// echo to work ram
	WORD mem_offset = (addr - (0x2000));
	if(mem_offset >= 0xc000 && mem_offset <= 0xcfff) 
		m_memory.wram_bank0[mem_offset & 0x0fff] = value;
	else if(mem_offset >= 0xd000 && mem_offset <= 0xdfff){
		m_memory.wram_bank1n[mem_offset & 0x0fff] = value;
	}

};

void Mem_mu::write_wram(WORD addr, BYTE value){
	if(addr >= 0xc000 && addr <= 0xcfff)
		m_memory.wram_bank0[addr & 0x1fff] = value;
	else if(addr >= 0xd000 && addr <= 0xdfff) 
		m_memory.wram_bank1n[addr & 0x1fff] = value;

	// echo to mirror work ram
	WORD mem_offset = addr + (1024 * 8);
	if(mem_offset <= 0xfdff){
		m_memory.wram_mirror[mem_offset & 0x1fff] = value;
	}
}

void Mem_mu::write(WORD addr, BYTE value){
	// dont allow writes to ROM memory
	if(addr <= 0x8000) return;
	else if(addr >= 0xfea0 && addr <= 0xfeff) return; //prohibited
	else if(addr >= 0xe000 && addr <= 0xfdff) write_wram_mirror(addr, value);
	else if(addr >= 0xc000 && addr <= 0xdfff) write_wram(addr, value);
};

