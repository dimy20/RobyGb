#include "Gb_ppu.h"

void Lcd::init(Mem_mu * memory){
	m_memory = memory;
}

BYTE Lcd::status() const { return m_memory->read(Mem_mu::io_port::STAT); };
BYTE Lcd::control() const { return m_memory->read(Mem_mu::io_port::LCDC); };
bool Lcd::enabled() const{ return control() >> 7;};
bool Lcd::window_enabled() const{ return control() & 0x20; };
WORD Lcd::bg_tilemap() const{ return control() & 0x08 ? 0x9c00 : 0x9800; };
WORD Lcd::wn_tilemap() const{ return control() & 0x40 ? 0x9c00 : 0x9800; };
BYTE Lcd::window_x() const { return m_memory->read(Mem_mu::io_port::WX); };
BYTE Lcd::window_y() const { return m_memory->read(Mem_mu::io_port::WY); };
BYTE Lcd::scroll_x() const { return m_memory->read(Mem_mu::io_port::SCX); };
BYTE Lcd::scroll_y() const { return m_memory->read(Mem_mu::io_port::SCY); };

void Lcd::fire_interrupt(intrp i) const {
	auto intrp_flag = m_memory->read(IF_ADDR);
	intrp_flag |= (0x1 << i);
	m_memory->write(IF_ADDR, intrp_flag);
};

void Lcd::set_mode(const int mode) const {
	assert(mode >=0 && mode <= 3);
	BYTE stat = status();

	stat &= 0xfc;
	stat |= mode;

	m_memory->write(Mem_mu::io_port::STAT, stat);

	if(mode == 3) return;

	// fire interrupt if sources for mode 2, 0 or 1 are set.
	if((stat & 0x20) || (stat & 0x10) || (stat & 0x08)){
		fire_interrupt(intrp::LCD);
	}
};

void Lcd::ly_compare(BYTE line){
	BYTE lyc = m_memory->read(Mem_mu::io_port::LYC);
	BYTE stat = status();

	if(line == lyc){
		stat |= 0x04;; // set coincidence
		if(stat & 0x40){
			fire_interrupt(intrp::LCD);
		}
	}
};

WORD Lcd::tile_data() const{
	return ((control() >> 4) & 1) ? 0x8000 : 0x9000;
};

void Gb_ppu::init(Mem_mu * memory, Window * window){
	m_memory = memory;
	m_window = window;
	m_viewport = std::vector<unsigned char>(VIEWPORT_WIDTH * VIEWPORT_HEIGHT * 3, 0xff);
	m_lcd.init(memory);
};

void Gb_ppu::draw_scanline(int line){
	render_tiles(line);
		/*
		return;
	BYTE control = m_memory->read(Mem_mu::io_port::LCDC);
	if(control & 0x1){

	};
	//if((control >> 1) & 0x1)
		render_sprites();
		*/
};

void Gb_ppu::update_graphics(int elapsed_cycles){

	// takes 70224 clicks to render a frame
	// takes 456 clicks to render an entire line
	int current_frame = elapsed_cycles % 70224; // upper bound because it takes less
	int line = current_frame / 456; // current line

	int clicks = current_frame % 456;

	// edge case
	if(!m_lcd.enabled()){
		// start drawing from line 0 again?
		// set mode to vblank (1)
		// return;
	};

	// direct access, writes to LY resets value back to 0.
	if(line != m_prev_line)
		m_memory->m_memory.io_registers[Mem_mu::io_port::LY - 0xff00] = line;

	//std::cout << line  << std::endl;
	if(line < 144){
		if(clicks <= 80){
			m_lcd.set_mode(Lcd::mode::OAM);
		}else if(clicks <= 289){
			m_lcd.set_mode(Lcd::mode::VRAM);
		}else if(clicks < 456){
			m_lcd.set_mode(Lcd::mode::HBLANK);
		};

		m_lcd.ly_compare(line);
	}

	if(line != m_prev_line && line < 144){
		draw_scanline(line);
		m_prev_line = line;
	};

	// Render frame
	if(line == 144){
		m_lcd.set_mode(Lcd::mode::VBLANKM);
		// send texture to opengl
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
			BYTE wx = m_lcd.window_x() - 7;
			BYTE wy = m_lcd.window_y();
			if(scanline >= wy && scanline - wy < 144 && pixel_x >= wx){
				// window pixel position relative to the viewport.
				curr_x = pixel_x - wx;
				tile_map = m_lcd.wn_tilemap();
				curr_y = scanline - wy;
			}
		}else{
			// pixel position in viewport relative to the background
			// Wraps around if exceeds edges.
			BYTE scy = m_lcd.scroll_y();
			BYTE scx = m_lcd.scroll_x();
			tile_map = m_lcd.bg_tilemap(); 

			curr_x = (pixel_x + scx) % 256;
			curr_y = (scanline + scy) % 256; 
		}

		// find the tile this pixel belongs to
		WORD tile_addr = pixel_find_tile(tile_map, curr_x, curr_y);
		// build the pixel
		BYTE pixel_value = tile_assemble_pixel(tile_addr, curr_x % 8, curr_y % 8); // get a pixel
		// get the final color from palette
		BYTE color = palette_get_color(pixel_value);
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

BYTE Gb_ppu::palette_get_color(BYTE color_index){
	BYTE lo_bit = 0, hi_bit = 0;

	lo_bit = (color_index * 2);
	hi_bit = (color_index * 2) + 1;

	BYTE palette = m_memory->read(Mem_mu::io_port::BGP);

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
// not implemented yet
void Gb_ppu::render_sprites(){};

