#pragma once

#include <iostream>
#include <cassert>
#include <vector>
#include "Memory.h"
#include "Window.h"

#define VIEWPORT_WIDTH 160
#define VIEWPORT_HEIGHT 144
#define DRAW_SCANLINE_CYCLES 456 // it takes 456 cycles to draw a scanline

// class to control lcdc and ldc stat
class Lcd{
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
		Lcd() = default;
		void init(Mem_mu * memory);

		void set_mode(const int mode) const;
		void ly_compare(BYTE line);
		void fire_interrupt(intrp i) const;
		WORD tile_data() const;
		BYTE status() const;
		BYTE control() const;

		BYTE window_x() const;
		BYTE window_y() const;

		BYTE scroll_x() const;
		BYTE scroll_y() const;

		WORD bg_tilemap() const;
		WORD wn_tilemap() const;
		WORD oam_base() const;
		BYTE obj_size() const;

		bool window_enabled() const;
		bool enabled() const;
		bool obj_enabled() const;
		bool wnbg_enabled() const; // window/background priority
	private:
		Mem_mu * m_memory;
};

class Gb_ppu{
	public:
		Gb_ppu() = default;
		void init(Mem_mu * memory, Window * window);
		void update_graphics(int cycles);
		enum intrp{
			VBLANK = 0,
			LCD,
			TIMER,
			SERIAL,
			JOYPAD
		};
	private:
		void draw_scanline(int line);
		bool enabled() const; // test 
		void render_tiles(int scanline);
		void render_sprites(int line);
		BYTE get_scanline() const{ return m_memory->read(0xff44); };
		WORD pixel_find_tile(const WORD tilemap, BYTE pixel_x, BYTE pixel_y);
		BYTE tile_assemble_pixel(WORD tile_addr, WORD x, WORD y);
		BYTE palette_get_color(BYTE color_id, WORD palette_addr);
		void set_lcdc_mode(const int mode) const;


		void tile_map_debug();

		void make_map();
		void build_tile(WORD tile_addr);
	private:
		Window * m_window = nullptr; // emulator graphic output layer
		Mem_mu * m_memory = nullptr;
		// keep track of how many cycles are left to finished drawing current scanline.
		int m_cycles_left = DRAW_SCANLINE_CYCLES;
		std::vector<unsigned char> m_viewport;
		int m_prev_line = -1;
		Lcd m_lcd;
};
