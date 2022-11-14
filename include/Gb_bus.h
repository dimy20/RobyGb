#pragma once
#include <unordered_map>
#include <functional>
#include "Memory.h"
#include "Gb_ppu.h"
#include "Gb_timer.h"
#include "Gb_interrupts.h"
#include "Gb_cartridge.h"

enum io_port{
	P1 = 0Xff00,
	SB = 0xff01,
	IF = 0xff0f, // x
	IE = 0xffff,  // x
	DIV = 0xff04, // x
	TIMA = 0xff05, // x
	TMA = 0xff06, // x
	TAC = 0Xff07, // x
	LY = 0xff44, // x
	LCDC = 0xff40, // x
	STAT = 0xff41, //x
	BGP = 0xff47, // x
	OBP0 = 0xff48, // x
	OBP1 = 0xff49, //x
	SCY = 0xff42, // x
	SCX = 0xff43, //x
	LYC = 0xff45, //x 
	WY = 0Xff4a, // x
	WX = 0xff4b, // x
	DMA = 0xff46 //x
};

struct io_registers{

};

class Gb_bus{
	public:
		Gb_bus() = default;
		void init(Mem_mu * memory, Gb_ppu * ppu, Gb_timer * timer, Gb_interrupts * intrs, Gb_lcd * lcd, Gb_cartridge * m_cart);
		void write(unsigned short addr, unsigned char value);
		unsigned char read(unsigned short addr);
	private:
		void build_wiomap();
		void build_riomap();
	private:
		Mem_mu * m_memory;
		Gb_ppu * m_ppu;
		Gb_timer * m_timer;
		Gb_interrupts * m_intrs;
		Gb_lcd * m_lcd;
		Gb_cartridge * m_cart;
		std::unordered_map<unsigned short, std::function<void(unsigned char)>> m_wmap;
		std::unordered_map<unsigned short, std::function<unsigned char(void)>> m_rmap;
		struct io_registers m_io;
};
