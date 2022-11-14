#pragma once
#include <unordered_map>
#include <functional>
#include <cassert>
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

class Gb_bus;
class Mem_mu{
	public:
		Mem_mu() = default;
		BYTE read(WORD addr) const;
		void write(WORD addr, BYTE value);
		void init(Gb_bus * bus);
		void update_DMA(int cycles);
		void debug();
		void setup_DMA(unsigned char value);
		constexpr int dma_pending() const { return m_dma_pending; };
		constexpr unsigned char dma_reg() const { return m_dma_reg; };
	private:
		void write_wram_mirror(WORD addr, BYTE value);
		void write_wram(WORD addr, BYTE value);
		BYTE read_wram_mirror(WORD addr) const ;
		void raise_restriction_hram_only(bool raise);
		void DMA_transfer(WORD DMA_src);
	private:
		Gb_bus * m_bus;
		unsigned char m_vram[1024 * 8];
		unsigned char m_hram[127];
		unsigned char m_wram[1024 * 4];
		unsigned char m_wram2[1024 * 4];
		unsigned char m_OAM[40 * 4];
		unsigned char m_dma_reg = 0;
		int m_dma_pending = 0;
		unsigned short m_DMA_src;
		bool m_dma_done = false;
		bool m_dma_restarted;
		int m_prev_cycles = 0;
		bool m_dma_lastcycle = false;
};
