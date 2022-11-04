#include "Memory.h"
#include <iostream>
#include <cstring>

Mem_mu::Mem_mu(){
	memset(&m_memory, 0, sizeof(struct Gb_memory));
    write(0xff10, 0x80);
    write(0xff11, 0xbf);
    write(0xff12, 0xf3);
    write(0xff14, 0xbf);
    write(0xff16, 0x3f);
    write(0xff19, 0xbf);
    write(0xff1a, 0x7f);
    write(0xff1b, 0xff);
    write(0xff1c, 0x9f);
    write(0xff1e, 0xbf);
    write(0xff20, 0xFf);
    write(0xff23, 0xbf);
    write(0xff24, 0x77);
    write(0xff25, 0xf3);
    write(0xff26, 0xf1);
    write(0xff40, 0x91);
    write(0xff47, 0xfc);
    write(0xff48, 0xff);
    write(0xff49, 0xff);
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
	if(addr == 0xffff){
		return m_memory.ie_register;
	};

	std::cout << "read unimplemented " << std::hex << (unsigned int)addr << std::endl;
	exit(1);
};

BYTE Mem_mu::read_wram_mirror(WORD addr) const {
	if(access_restricted(mem_region::WRAM)) return 0xff;

	WORD offset = addr - 0x2000;
	if(offset >= 0xc000 && offset <= 0xcfff)
		return m_memory.wram_bank0[offset - 0xc000];
	else{
		return m_memory.wram_bank1n[offset - 0xd000];
	}
}

void Mem_mu::write_wram_mirror(WORD addr, BYTE value){
	if(access_restricted(mem_region::WRAM)) return;

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
	//if(access_restricted(mem_region::WRAM)) return;

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
	if(addr <= 0x7fff){
		if(access_restricted(mem_region::ROM)) return;
		m_cart->mbc_intercept(addr, value);
	}
	else if(addr >= 0x8000 && addr <= 0x9fff){
		//if(access_restricted(mem_region::VRAM)) return;
		m_memory.vram[addr - 0x8000] = value;
	}
	else if(addr >= 0xa000 && addr <= 0xbfff){
		if(access_restricted(mem_region::EXRAM)) return;
		m_cart->write(addr, value);
	}
	else if(addr >= 0xfea0 && addr <= 0xfeff) return; //prohibited
	else if(addr >= 0xe000 && addr <= 0xfdff) write_wram_mirror(addr, value);
	else if(addr >= 0xc000 && addr <= 0xdfff) write_wram(addr, value);
	else if(addr >= 0xff00 && addr <= 0xff7f) handle_io_ports(addr, value);
	else if(addr >= 0xff80 && addr <= 0xfffe){ // high ram
		m_memory.hram[addr - 0xff80] = value;
	}else if(addr == 0xffff){
		m_memory.ie_register = value;
	}else{
		std::cout << "write unimplemented " << std::hex << (unsigned int)addr << std::endl;
		exit(1);
	}
};

void Mem_mu::init(Gb_cartridge * cart, Gb_timer * timer){
	m_cart = cart;
	m_memory.io_registers[0x0044] = 0x90;
	m_timer = timer;
};

void Mem_mu::handle_io_ports(WORD addr, BYTE value){
	// attemp to write to current scanline, resets it.
	if(addr == io_port::LY){
		m_memory.io_registers[addr - 0xff00] = 0;
		return;
	}else if(addr == io_port::DIV){ // divider
		m_memory.io_registers[addr - 0xff00] = 0;
		return;
	}else if(addr == io_port::TAC){ // timer controller
		auto prev_freq = read(io_port::TAC);
		m_memory.io_registers[addr - 0xff00] = value;

		if((prev_freq & 0x3) != (value & 0x3)){
			m_timer->update_frequency();
		}
		return;
	}else if(addr == 0xff46){ // DMA transfer
		// raise access restriction to hram only
		m_access_restrictions = 0xfe;
		// since this byte holds src/0x100, multiply to get original src addr
		// DMA transfer takes 160 cycles and during this time the cpu can only
		// acess HRAM.
		WORD src_addr = value * 0x100;
		for(int i = 0; i < 160; i++){
			m_memory.sprite_attr_table[i] = read(src_addr + i);
		};
		return;
	}else if(addr >= 0xff10 && addr <= 0xff26){
		// not implemented (sound modes and waves patterns)
		return;
	}else if(addr == io_port::SB){
		m_memory.io_registers[0xff01 - 0xff00] = value;
		std::cout << value << " ";
		return;
	}else{
		m_memory.io_registers[addr - 0xff00] = value;
	}
};
