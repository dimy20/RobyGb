#include "Gb_ppu.h"

void Gb_ppu::init(){};

void Gb_ppu::draw_scanline(){
	std::cout << "to be implemented..." << std::endl;
};

void Gb_ppu::update_graphics(int elapsed_cycles){
	update_lcd_status();

	m_cycles_left -= elapsed_cycles;

	if(!enabled() || m_cycles_left > 0) return;

	auto memory = m_memory->data();
	auto curr_line = ++memory->io_registers[LY_ADDR - 0xff00];

	m_cycles_left = DRAW_SCANLINE_CYCLES;
	// vertical blank
	if(curr_line >= 144){
		signal_interrupt(intrp::VBLANK);
	}else if(curr_line > 153){
		memory->io_registers[LY_ADDR - 0xff00] = 0;
	}else if(curr_line >= 0 && curr_line <= 143){
		draw_scanline();
	};
};

bool Gb_ppu::enabled() const{ 
	auto lcdc_reg = m_memory->read(LCDC); 
	return (lcdc_reg >> 7) & 0x1;
};

void Gb_ppu::update_lcd_status(){
	BYTE status = m_memory->read(STAT_ADDR); 
	/*If lcd is disabled, reset scanline and put mode 1 (vblank) back.*/
	if(!enabled()){
		m_cycles_left = DRAW_SCANLINE_CYCLES;

		// set current scanline number back to 0
		auto memory = m_memory->data();
		memory->io_registers[LY_ADDR - 0xff00] = 0;

		status = set_lcd_mode(status, 1);
		m_memory->write(STAT_ADDR, status);

		return;
	};

	BYTE prev_mode = status & 0x3;

	int mode2_lower_bound = DRAW_SCANLINE_CYCLES - 80; // mode 2 duration is 80 dots.
	// mode 3 duration is 168 - 292 dots, depending on sprite count
	// TODO: update this value dynamically depending on sprite count.
	int mode3_lower_bound = DRAW_SCANLINE_CYCLES - 168; 

	auto curr_line = m_memory->read(LY_ADDR);
	bool req_intrp = false;

	// fire interrupt only when state changes to mode 1, 0 or 2.
	if(curr_line >= 144){
		// TODO: raise all restriction
		status = set_lcd_mode(status, 1);
		req_intrp = (status >> 4) & 0x1;
	}else if(m_cycles_left >= mode2_lower_bound){
		// TODO: raise all restriction but oam
		status = set_lcd_mode(status, 2);
		req_intrp = (status >> 5) & 0x1;
	}else if(m_cycles_left >= mode3_lower_bound){
		//TODO : oam restrict and all of vram
		status = set_lcd_mode(status, 3);
	}else{
		// TODO: raise all restriction
		status = set_lcd_mode(status, 0);
		req_intrp = (status >> 3) & 0x1;
	}

	if(req_intrp && (prev_mode != (status & 0x3)))
		signal_interrupt(intrp::LCD);

	if(curr_line == m_memory->read(0xff45)){
		status |= 0x1 << 2; // coincidence bit
		if((status >> 6) & 0x1)
			signal_interrupt(intrp::LCD);
	}else{
		status &= ~(0x1 << 2);
	}

	m_memory->write(STAT_ADDR, status);
};

void Gb_ppu::signal_interrupt(intrp i){
	auto intrp_flag = m_memory->read(IF_ADDR);
	intrp_flag |= (0x1 << i);
	m_memory->write(IF_ADDR, intrp_flag);
};


BYTE Gb_ppu::set_lcd_mode(BYTE status, int mode){
	assert(mode <= 3);
	status &= 0xfc;
	status |= mode;
	return status;
};
