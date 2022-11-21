#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include "Memory.h"
#include "Window.h"
#include "Gb_interrupts.h"

#define VIEWPORT_WIDTH 160
#define VIEWPORT_HEIGHT 144
#define DRAW_SCANLINE_CYCLES 456 // it takes 456 cycles to draw a scanline

// class to control lcdc and ldc stat
class Gb_lcd{
	public:
		enum mode{
			HBLANK = 0,
			VBLANKM = 1,
			OAM = 2,
			VRAM = 3,
		};
		enum intrp{
			VBLANK = 0,
			LCD,
			TIMER,
			SERIAL,
			JOYPAD
		};
		Gb_lcd() = default;
		void init(Mem_mu * memory, Gb_interrupts * intrs);

		constexpr void set_mode(const int mode);
		void ly_compare(BYTE line);
		WORD tile_data() const;
		BYTE obj_size() const;

		constexpr void set_STAT(unsigned char value){ m_stat = value; };
		constexpr void set_LCDC(unsigned char value){ m_control = value; };
		constexpr void set_SCX(unsigned char value) { m_scx = value; };
		constexpr void set_SCY(unsigned char value) { m_scy = value; };
		constexpr void set_WX(unsigned char value) { m_wx = value; };
		constexpr void set_WY(unsigned char value) { m_wy = value; };
		constexpr void set_BGP(unsigned char value) { m_bgp = value; };
		constexpr void set_OBP0(unsigned char value) { m_obp0 = value; };
		constexpr void set_OBP1(unsigned char value) { m_obp1 = value; };
		constexpr void set_LYC(unsigned char value) { m_lyc = value; };

		constexpr BYTE status() const { return m_stat; };
		constexpr BYTE control() const { return m_control; };
		constexpr BYTE scx() const { return m_scx; };
		constexpr BYTE scy() const { return m_scy; };
		constexpr BYTE wx() const { return m_wx; };
		constexpr BYTE wy() const { return m_wy; };
		constexpr BYTE BGP() {  return m_bgp; };
		constexpr BYTE OBP0() { return m_obp0; };
		constexpr BYTE OBP1() { return m_obp1; };
		constexpr BYTE LYC() { return m_lyc; };


		constexpr bool enabled() const{ return m_control >> 7;};
		constexpr bool window_enabled() const{ return m_control & 0x20; };
		constexpr bool obj_enabled() const {return m_control & 0x02; };
		constexpr bool wnbg_enabled() const {return m_control & 1; };

		constexpr WORD bg_tilemap() const{ return m_control & 0x08 ? 0x9c00 : 0x9800; };
		constexpr WORD wn_tilemap() const{ return m_control & 0x41 ? 0x9c00 : 0x9800; };

		constexpr unsigned char mode () const { return m_stat & 3; };
	private:
		Mem_mu * m_memory;
		Gb_interrupts * m_intrs;
		unsigned char m_stat; // 0xff41 -> stat register
		unsigned char m_control = 0x91; // 0xff40 -> lcdc register
		unsigned char m_scx;  // 0xff43 -> scroll x register
		unsigned char m_scy; // 0xff42 -> scroll y register
		unsigned char m_wx;  // 0xff4b -> window x register
		unsigned char m_wy; // 0xff4a -> window y register
		unsigned char m_bgp = 0xfc; // 0xff47 -> background palette
		unsigned char m_obp0 = 0xff; // 0xff48 -> palette registers
		unsigned char m_obp1 = 0xff; //0xff49 -> platter register
		unsigned char m_lyc; // 0xff45 -> lyc register
};

class Gb_ppu{
	public:
		Gb_ppu() = default;
		void init(Mem_mu * memory, Window * window, Gb_interrupts * intrs);
		void update_graphics(int cycles);
		constexpr BYTE line() const { return m_line; };
		constexpr void reset_line() { m_line = 0; m_reset = true; };
		Gb_lcd * lcd () { return &m_lcd; };

	private:
		void draw_scanline(int line);
		bool enabled() const; // test 
		void render_tiles(int scanline);
		void render_sprites(int line);
		BYTE get_scanline() const{ return m_memory->read(0xff44); };
		WORD pixel_find_tile(const WORD tilemap, BYTE pixel_x, BYTE pixel_y);
		BYTE tile_assemble_pixel(WORD tile_addr, WORD x, WORD y);
		BYTE palette_get_color(BYTE color_id, unsigned char palette);


		void make_map();
		void build_tile(WORD tile_addr);
	private:
		Window * m_window = nullptr; // emulator graphic output layer
		Mem_mu * m_memory = nullptr;
		Gb_interrupts * m_intrs;
		Gb_lcd m_lcd;
		// keep track of how many cycles are left to finished drawing current scanline.
		int m_cycles_left = DRAW_SCANLINE_CYCLES;
		std::vector<unsigned char> m_viewport;
		int m_line = 0;
		int m_prev_line = -1;

		bool m_reset = false;
		int m_mode = 2;
};
