#pragma once
#include <unordered_map>
#include <functional>
#include <cassert>
#include "Gb_types.h"
#include "Gb_cartridge.h"
#include "Gb_timer.h"
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

#define IF_ADDR 0xff0f // interrupt flag (request)
#define IE_ADDR 0xffff // interrupt enable
#define STAT_ADDR 0xff41

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

enum mem_region{
	HRAM = 0,
	WRAM,
	ROM,
	VRAM,
	EXRAM,
	OAM,
	REGISTERS,
	IE
};

class Gb_ppu;
// Memory managment unit.
class Mem_mu{
	friend Gb_ppu;
	friend Gb_timer;
	public:
	enum io_port{
		P1 = 0Xff00,
		SB = 0xff01,
		IF = 0xff0f,
		IE = 0xffff,
		DIV = 0xff04,
		TIMA = 0xff05,
		TMA = 0xff06,
		TAC = 0Xff07,
		LY = 0xff44,
		LCDC = 0xff40,
		STAT = 0xff41,
		BGP = 0xff47,
		OBP0 = 0xff48,
		OBP1 = 0xff49,
		SCY = 0xff42,
		SCX = 0xff43,
		LYC = 0xff45,
		WY = 0Xff4a,
		WX = 0xff4b,
	};
	public:
		Mem_mu();
		BYTE read(WORD addr) const;
		void write(WORD addr, BYTE value);
		Gb_cartridge * cart() { return m_cart; };
		void init(Gb_cartridge * cart, Gb_timer * timer);
	protected:
		Gb_memory * data(){ return &m_memory; };
	private:
		void write_wram_mirror(WORD addr, BYTE value);
		void write_wram(WORD addr, BYTE value);
		BYTE read_wram_mirror(WORD addr) const ;
		void handle_io_ports(WORD addr, BYTE value);
		void raise_restriction_hram_only(bool raise);
		constexpr bool access_restricted(mem_region r) const {return ( m_access_restrictions >> r) & 0x1;};
	private:
		struct Gb_memory m_memory;

		Gb_cartridge * m_cart = nullptr;
		Gb_timer * m_timer = nullptr;
		BYTE m_access_restrictions = 0;
};
