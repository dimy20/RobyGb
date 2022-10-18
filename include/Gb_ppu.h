#pragma once

#include <iostream>
#include "Memory.h"

#define DRAW_SCANLINE_CYCLES 456 // it takes 456 cycles to draw a scanline

class Gb_ppu{
	public:
		Gb_ppu(Mem_mu * memory) : m_memory(memory) {};
		void init();
		void update_graphics(int cycles);
		enum intrp{
			VBLANK = 0,
			LCD,
			TIMER,
			SERIAL,
			JOYPAD
		};
	private:
		void draw_scanline();
		bool enabled() const; // test 
		void update_lcd_status();
		void signal_interrupt(intrp i);
		BYTE set_lcd_mode(BYTE status, int i);
	private:
		Mem_mu * m_memory;
		// keep track of how many cycles are left to finished drawing current scanline.
		int m_cycles_left = DRAW_SCANLINE_CYCLES;
};
