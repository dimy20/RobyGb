#include "Memory.h"
#include <iostream>
#include <cstring>

Mem_mu::Mem_mu(){
	// just for now
	memset(&m_memory, 0, sizeof(struct Gb_memory));
};

BYTE Mem_mu::read(WORD addr) const {
	assert(m_cart != nullptr);

	if(addr <= 0x7fff) return m_cart->read_rom(addr);
	if(addr >= 0x8000 && addr <= 0x9fff) return m_memory.vram[addr - 0x8000];
	if(addr >= 0xa000 && addr <= 0xbfff) return m_cart->read_ram(addr);
	if(addr >= 0xc000 && addr <= 0xcfff) return m_memory.wram_bank0[addr - 0xc000];
	if(addr >= 0xd000 && addr <= 0xdfff) return m_memory.wram_bank1n[addr - 0xd000];
	if(addr >= 0xe000 && addr <= 0xfdff) return read_wram_mirror(addr);
	if(addr >= 0xfea0 && addr <= 0xfeff) return 0x00;
	if(addr >= 0xff00 && addr <= 0xff7f) return m_memory.io_registers[addr - 0xff00];
	if(addr >= 0xff80 && addr <= 0xfffe) return m_memory.hram[addr - 0xff80];
	std::cout << "read unimplemented " << std::endl;
};

BYTE Mem_mu::read_wram_mirror(WORD addr) const {
	WORD offset = addr - 0x2000;
	if(offset >= 0xc000 && offset <= 0xcfff)
		return m_memory.wram_bank0[offset - 0xc000];
	else{
		return m_memory.wram_bank1n[offset - 0xd000];
	}
}

void Mem_mu::write_wram_mirror(WORD addr, BYTE value){
	m_memory.wram_mirror[addr - 0xe000] = value;
	// echo to work ram
	WORD mem_offset = (addr - (0x2000));
	if(mem_offset >= 0xc000 && mem_offset <= 0xcfff) 
		m_memory.wram_bank0[mem_offset - 0xc000] = value;
	else if(mem_offset >= 0xd000 && mem_offset <= 0xdfff){
		m_memory.wram_bank1n[mem_offset - 0xd000] = value;
	}

};

void Mem_mu::write_wram(WORD addr, BYTE value){
	if(addr >= 0xc000 && addr <= 0xcfff)
		m_memory.wram_bank0[addr - 0xc000] = value;
	else if(addr >= 0xd000 && addr <= 0xdfff) 
		m_memory.wram_bank1n[addr - 0xd000] = value;

	// echo to mirror work ram
	WORD mem_offset = addr + (1024 * 8);
	if(mem_offset <= 0xfdff){
		m_memory.wram_mirror[mem_offset - 0xe000] = value;
	}
}

void Mem_mu::write(WORD addr, BYTE value){
	// dont allow writes to ROM memory
	if(addr <= 0x7fff) m_cart->mbc_intercept(addr, value);
	else if(addr >= 0x8000 && addr <= 0x9fff){
		m_memory.vram[addr - 0x8000] = value;
	}
	else if(addr >= 0xa000 && addr <= 0xbfff) m_cart->write(addr, value);
	else if(addr >= 0xfea0 && addr <= 0xfeff) return; //prohibited
	else if(addr >= 0xe000 && addr <= 0xfdff) write_wram_mirror(addr, value);
	else if(addr >= 0xc000 && addr <= 0xdfff) write_wram(addr, value);
	else if(addr >= 0xff00 && addr <= 0xff7f){ // io registers
		m_memory.io_registers[addr - 0xff00] = value;
	}else if(addr >= 0xff80 && addr <= 0xfffe){ // high ram
		m_memory.hram[addr - 0xff80] = value;
	}else if(addr == 0xffff){
		m_memory.ie_register = value;
	}else{
		std::cout << "Unimplemented yet" << std::endl;
	}
};

void Mem_mu::init(Gb_cartridge * cart){
	m_cart = cart;
	m_memory.io_registers[0x0044] = 0x90;
};
