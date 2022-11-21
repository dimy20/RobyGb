#include "Gb_ppu.h"

void Gb_lcd::init(Mem_mu * memory, Gb_interrupts * intrs){
	m_memory = memory;
	m_intrs = intrs;
}

BYTE Gb_lcd::obj_size() const{ return (control() >> 2) & 1; };

constexpr void Gb_lcd::set_mode(const int mode) {
	assert(mode >=0 && mode <= 3);
	m_stat &= 0xfc;
	m_stat |= mode;
};



void Gb_lcd::ly_compare(BYTE line){
	if(line == m_lyc){
		if(m_stat & 0x4) return;
		m_stat |= 0x04;; // set coincidence
		if(m_stat & 0x40){
			m_intrs->request(1);
		}
	}else{
		m_stat &= ~0x04;
	}
};

WORD Gb_lcd::tile_data() const{
	return ((control() >> 4) & 1) ? 0x8000 : 0x9000;
};

void Gb_ppu::init(Mem_mu * memory, Window * window, Gb_interrupts * intrs){
	m_memory = memory;
	m_window = window;
	m_intrs = intrs;
	m_viewport = std::vector<unsigned char>(VIEWPORT_WIDTH * VIEWPORT_HEIGHT * 3, 0xff);
	m_lcd.init(memory, m_intrs);
};

void Gb_ppu::draw_scanline(int line){
	unsigned char control = m_lcd.control();
	if(control & 1)
		render_tiles(line);
	if(control & 2)
		render_sprites(line);
};

void Gb_ppu::update_graphics(int elapsed_cycles){
	// takes 70224 clicks to render a frame
	// takes 456 clicks to render an entire line
	int current_frame = m_reset ? 0 : elapsed_cycles % 70224;
	if(m_reset) m_reset = false;

	m_line = current_frame / 456; // current line

	int clicks = current_frame % 456;

	// edge case
	if(!m_lcd.enabled()){
		reset_line();
		m_lcd.set_mode(Gb_lcd::mode::VBLANKM);
		return;
		// start drawing from line 0 again?
		// set mode to vblank (1)
		// return;
	};

	int prev_mode = m_mode;

	if(m_line < 144){
		if(clicks <= 80){
			m_lcd.set_mode(Gb_lcd::mode::OAM);
			m_mode = 2;
		}else if(clicks <= 289){
			m_lcd.set_mode(Gb_lcd::mode::VRAM);
			m_mode = 3;
		}else if(clicks < 456){
			m_lcd.set_mode(Gb_lcd::mode::HBLANK);
			m_mode = 0;
		};

		m_lcd.ly_compare(m_line); // TODO this just fire the interrupt only once too.


	}else if(m_line >= 144){
		m_mode = 1;
	}
	// mode changed, request lcd interrupt
	if(prev_mode != m_mode){
		auto stat = m_lcd.status();
		if(m_mode == 2 && (stat & 0x20)){
			m_intrs->request(1);
		}else if(m_mode == 0 && (stat & 0x08)){
			m_intrs->request(1);
		}else if(m_mode == 1 && (stat & 0x10)){
			m_intrs->request(1);
		}
	}

	if(m_line != m_prev_line && m_line < 144){
		draw_scanline(m_line);
		m_prev_line = m_line;
	};

	// Etering Vblank, Render frame
	if(m_prev_line != m_line && m_line == 144){
		/*If bit 5 (mode 2 oam interrupt) is set when vblank starts an interrupts is also triggered*/
		int stat = m_lcd.status();
		if(stat & 0x20)
			m_intrs->request(1);

		m_lcd.set_mode(Gb_lcd::mode::VBLANKM);
		m_intrs->request(0);
		m_window->ppu_recv(m_viewport);
	}

};

void Gb_ppu::render_tiles(int scanline){
	// draw each pixel of the viewport for the current scanline.
	for(BYTE pixel_x = 0; pixel_x < VIEWPORT_WIDTH; pixel_x++){
		WORD tile_map;
		auto offset = ((scanline * VIEWPORT_WIDTH) + pixel_x) * 3;
		WORD curr_x, curr_y;

		if(!m_lcd.enabled()){
			// when display is disabled the screen is blank
			m_viewport[offset] = 0;
			m_viewport[offset + 1] = 0;
			m_viewport[offset + 2] = 0;
		}else if(m_lcd.window_enabled()){
			// top left position -> (7, 0);
			BYTE wx = m_lcd.wx() - 7;
			BYTE wy = m_lcd.wy();
			if(scanline >= wy && scanline - wy < 144 && pixel_x >= wx){
				// window pixel position relative to the viewport.
				curr_x = pixel_x - wx;
				tile_map = m_lcd.wn_tilemap();
				curr_y = scanline - wy;
			}
		}else{
			// pixel position in viewport relative to the background
			// Wraps around if exceeds edges.
			BYTE scy = m_lcd.scy();
			BYTE scx = m_lcd.scx();
			tile_map = m_lcd.bg_tilemap(); 

			curr_x = (pixel_x + scx) % 256;
			curr_y = (scanline + scy) % 256; 
		}

		// find the tile this pixel belongs to
		WORD tile_addr = pixel_find_tile(tile_map, curr_x, curr_y);
		// build the pixel
		BYTE pixel_value = tile_assemble_pixel(tile_addr, curr_x % 8, curr_y % 8); // get a pixel
		// get the final color from palette
		BYTE color = palette_get_color(pixel_value, m_lcd.BGP());
		BYTE red, green, blue;

		switch (color){
			case 0: red = 0xff; green = 0xff; blue = 0xff; break; // white
			case 1: red = 0xcc; green = 0xcc; blue = 0xcc; break; // light grey
			case 2: red = 0x77; green = 0x77; blue = 0x77; break; // dark grey
			case 3: red = green = blue = 0; break; // black
		}
		m_viewport[offset] = red;
		m_viewport[offset + 1] = green;
		m_viewport[offset + 2] = blue;
	};

};


BYTE Gb_ppu::tile_assemble_pixel(WORD tile_addr, WORD x, WORD y){
	BYTE offset = (y * 2); // offset within 16 bytes of this tile

	BYTE b1 = m_memory->read(tile_addr + offset); // constains low bits
	BYTE b2 = m_memory->read(tile_addr + offset + 1); // contains high bits

	BYTE bit = 7 - x; // x coord in this line

	BYTE lo = (b1 >> bit) & 1;
	BYTE hi = ((b2 >> bit) & 1) << 1;
	
	return lo | hi;
};

BYTE Gb_ppu::palette_get_color(BYTE color_index, unsigned char palette){
	BYTE lo_bit = 0, hi_bit = 0;

	lo_bit = (color_index * 2);
	hi_bit = (color_index * 2) + 1;

	BYTE color = ((palette >> lo_bit) & 1) | (((palette >> hi_bit) & 1) << 1);

	return color;
};

// find a the tile addr for a given pixel (x, y) in the tilemap given.
WORD Gb_ppu::pixel_find_tile(const WORD tilemap, BYTE pixel_x, BYTE pixel_y){
	// location of tile number in map
	BYTE tile_num_x = pixel_x / 8;
	BYTE tile_num_y = pixel_y / 8;

	WORD tile_num_addr = tilemap + ((tile_num_y * 32) + tile_num_x);
	// get tile num
	BYTE tile_num = m_memory->read(tile_num_addr);

	WORD tile_data = m_lcd.tile_data();
	bool signed_adressing = tile_data == 0x9000;

	// offset of tile in tile data region
	WORD offset;
	if(!signed_adressing){
		offset = tile_data + (tile_num * 16);
	}else{
		offset = tile_data + (static_cast<SIGNED_BYTE>(tile_num) * 16);
	}

	return offset;
};

void Gb_ppu::render_sprites(int line){
	if(!m_lcd.obj_enabled()) return;

	WORD sprite_table = 0xfe00;
	const WORD tile_data = 0x8000;

	for(int sprite = 0; sprite < 40; sprite++){

		auto offset = sprite_table + (sprite * 4);

		BYTE y_position = m_memory->read(offset) - 16;
		BYTE x_position = m_memory->read(offset + 1) - 8;
		BYTE tile_index = m_memory->read(offset + 2);
		BYTE attribs = m_memory->read(offset + 3);

		bool x_flip = (attribs >> 5) & 1;
		bool y_flip = (attribs >> 6) & 1;

		int width = m_lcd.obj_size() ? 16 : 8; // 8x8 or 8x16 sprites
		// does the current line intercept with this sprite?
		if(line >= y_position && line < y_position + width){
			auto sprite_row = line - y_position;
			sprite_row = y_flip ? (sprite_row - width) * -1 : sprite_row; // if y_flip read from bottom to top.

			WORD tile_addr = (tile_data + (tile_index * 16)) + (sprite_row * 2);

			BYTE b1 = m_memory->read(tile_addr);
			BYTE b2 = m_memory->read(tile_addr + 1);

			for(int pixel = 7; pixel >= 0; pixel--){
				int bit = x_flip ? (pixel - 7) * -1 : pixel; // if x_flip read from right to left

				int lo = (b1 >> bit) & 1;
				int hi = ((b2 >> bit) & 1) << 1;

				BYTE color_id = lo | hi;

				unsigned char palette;
				palette = (attribs >> 4) & 1 ? m_lcd.OBP1() : m_lcd.OBP0();

				BYTE pixel_color = palette_get_color(color_id, palette);

				if(pixel_color == 0) continue;

				BYTE red, green, blue;

				switch (pixel_color){
					case 1: red = 0xcc; green = 0xcc; blue = 0xcc; break; // light grey
					case 2: red = 0x77; green = 0x77; blue = 0x77; break; // dark grey
					case 3: red = green = blue = 0; break; // black
				}

				int pixel_x = x_position + ((pixel - 7) * -1); // map [7, 0] to [0, 7]
				auto vp_offset = ((line * VIEWPORT_WIDTH) + pixel_x) * 3;

				m_viewport[vp_offset] = red;
				m_viewport[vp_offset + 1] = green;
				m_viewport[vp_offset + 2] = blue;

			};

		};
	};
};
