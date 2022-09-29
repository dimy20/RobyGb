#pragma once
#include "Gb_types.h"
/* General Memory Map
 * The Gameboy has a 16 bit address bus, it is used for ROM, RAM and I/O.
 *
 * <0000-------------3FFF> 16kb ROM bank 00 : From cartdrige, is usually a fixed bank
 * <4000-------------7FFF> 16kb ROM bann 01-nn: From cartdrige, switchable via MBC.
 * <8000-------------9FFF> 8kb Video ram(vram): Only bank 0 in Non-CGB mode
 *												Switchable bank 0/1 in CGB mode.
 * <A000-------------BFFF> 8kb external RAM: In cartdrige, swithcable bank if any.
 * <C000-------------CFFF> 4kb work ram(wram): In cartdrige, swithcable bank if any.
 * <D000-------------DFFF> 4kb work ram(wram) bank1-n: Only bank 1 in non-cgb mode
 *													   Switchable in bank 1-6 in cgb
 *													   mode.
 * <E000-------------FDFF> Mirror of C000-DDFF(echo ram):Use of this area is prohibited.
 * <FE00-------------FE9F> Sprite attribute table.(OAM)
 * <FEA0-------------FEFF> Not usable, use of this area is prohibited.
 * <FF00-------------FF7F> I/0 registers.
 * <FF80-------------FFFE> High ram(HRAM)
 * <FFFF-------------FFFF> Interrupt enable register(IE)
 *
 * */

#define ROM_BANK_SIZE 1024 * 16 // 16kb
#define VRAM_SIZE 1024 * 8 // 8kb
#define WORK_RAM_SIZE 1024 * 4
#define SPRITE_ATTR_SIZE 40 * 4
#define UNUSED_REGION_SIZE 96
#define IO_RANGE_SIZE 128
#define HRAM_SIZE 127

struct Gb_memory{
	BYTE rom_bank0[ROM_BANK_SIZE];
	BYTE rom_bank1n[ROM_BANK_SIZE];
	BYTE vram[VRAM_SIZE];
	BYTE ex_ram[VRAM_SIZE];
	BYTE wram_bank0[WORK_RAM_SIZE];
	BYTE wram_bank1n[WORK_RAM_SIZE];
	BYTE wram_mirror[(WORK_RAM_SIZE * 2) - 512]; // prohibited
	BYTE sprite_attr_table[SPRITE_ATTR_SIZE];
	BYTE unused_region[UNUSED_REGION_SIZE]; // prohibited
	BYTE io_registers[IO_RANGE_SIZE];
	BYTE hram[HRAM_SIZE];
	BYTE ie_register;
};

// Memory managment unit.
class Mem_mu{
	public:
		Mem_mu();
		BYTE read(WORD addr);
		void write(WORD addr, BYTE value);
	private:
		void write_wram_mirror(WORD addr, BYTE value);
		void write_wram(WORD addr, BYTE value);
		BYTE read_wram_mirror(WORD addr);
	private:
		struct Gb_memory m_memory;
		// memory banking controller
		bool m_mbc1 = false; 
		bool m_mbc2 = false;
};
