#include "Gb_bus.h"
#include <cassert>

void Gb_bus::build_wiomap(){
	// sys timer
	m_wmap[io_port::TIMA] = [this](unsigned char value){ m_timer->set_TIMA(value); };
	m_wmap[io_port::DIV] = [this](unsigned char value){ (void)value; m_timer->set_DIV(); };
	m_wmap[io_port::TAC] = [this](unsigned char value){ m_timer->set_TAC(value); };
	m_wmap[io_port::TMA] = [this](unsigned char value){ m_timer->set_TMA(value); };
	// interrupts
	m_wmap[io_port::IF] = [this](unsigned char value){ m_intrs->set_IF(value); };
	m_wmap[io_port::IE] = [this](unsigned char value){ m_intrs->set_IE(value); };

	m_wmap[io_port::LY] = [this](unsigned char value){ (void)value; m_ppu->reset_line(); };
	m_wmap[io_port::DMA] = [this](unsigned char value){ m_memory->setup_DMA(value); };
	// lcd registers
	m_wmap[io_port::STAT] = [this](unsigned char value){ m_lcd->set_STAT(value); };
	m_wmap[io_port::LCDC] = [this](unsigned char value){ m_lcd->set_LCDC(value); };
	m_wmap[io_port::SCX] = [this](unsigned char value){ m_lcd->set_SCX(value); };
	m_wmap[io_port::SCY] = [this](unsigned char value){ m_lcd->set_SCY(value); };
	m_wmap[io_port::WX] = [this](unsigned char value){ m_lcd->set_WX(value); };
	m_wmap[io_port::WY] = [this](unsigned char value){ m_lcd->set_WY(value); };
	m_wmap[io_port::BGP] = [this](unsigned char value){ m_lcd->set_BGP(value); };
	m_wmap[io_port::OBP0] = [this](unsigned char value){ m_lcd->set_OBP0(value); };
	m_wmap[io_port::OBP1] = [this](unsigned char value){ m_lcd->set_OBP1(value); };
	m_wmap[io_port::LYC] = [this](unsigned char value){ m_lcd->set_LYC(value); };

};

void Gb_bus::build_riomap(){
	m_rmap[io_port::TIMA] = [this]()-> unsigned char{ return m_timer->TIMA(); };
	m_rmap[io_port::DIV] = [this]()-> unsigned char{  return m_timer->DIV(); };
	m_rmap[io_port::TAC] = [this]()-> unsigned char{  return m_timer->TAC(); };
	m_rmap[io_port::TMA] = [this]()-> unsigned char{  return m_timer->TMA(); };

	m_rmap[io_port::IF] = [this]()-> unsigned char { return m_intrs->IF(); };
	m_rmap[io_port::IE] = [this]()-> unsigned char { return m_intrs->IE(); };

	m_rmap[io_port::LY] = [this]()-> unsigned char { return m_ppu->line(); };
	m_rmap[io_port::DMA] = [this]()-> unsigned char{ return m_memory->dma_reg(); };

	m_rmap[io_port::STAT] = [this]()-> unsigned char{ return m_lcd->status(); };
	m_rmap[io_port::LCDC] = [this]()-> unsigned char{ return m_lcd->control(); };
	m_rmap[io_port::SCX]  = [this]()-> unsigned char{ return m_lcd->scx(); };
	m_rmap[io_port::SCY]  = [this]()-> unsigned char{ return m_lcd->scy(); };
	m_rmap[io_port::WX]   = [this]()-> unsigned char{ return m_lcd->wx(); };
	m_rmap[io_port::WY]   = [this]()-> unsigned char{ return m_lcd->wy(); };
	m_rmap[io_port::BGP]  = [this]()-> unsigned char{ return m_lcd->BGP(); };
	m_rmap[io_port::OBP0] = [this]()-> unsigned char{ return m_lcd->OBP0(); };
	m_rmap[io_port::OBP1] = [this]()-> unsigned char{ return m_lcd->OBP1(); };
	m_rmap[io_port::LYC]  = [this]()-> unsigned char{ return m_lcd->LYC(); };
};

void Gb_bus::init(Mem_mu * memory, Gb_ppu * ppu, Gb_timer * timer, Gb_interrupts * intrs, 
		Gb_lcd * lcd, Gb_cartridge * cart){
	m_timer = timer;
	m_memory = memory;
	m_ppu = ppu;
	m_intrs = intrs;
	m_lcd = lcd;
	m_cart = cart;
	build_wiomap();
	build_riomap();
};

void Gb_bus::write(unsigned short addr, unsigned char value){;
	// only high ram access during DMA.
	if(m_memory->dma_pending() && ( addr < 0xff80 && addr > 0xfffe )) return;
	else if(addr <= 0x7fff) m_cart->mbc_intercept(addr, value);
	else if(addr >= 0xa000 && addr <= 0xbfff) m_cart->write(addr, value);
	else if(addr == 0xffff) m_intrs->set_IE(value);
	else if(addr >= 0xff00 && addr <= 0xff7f){ // registers
		if(m_wmap.find(addr) != m_wmap.end()){
			m_wmap[addr](value);
		}else{
			//std::cout << "I/O register " << std::hex << addr << " not supported." << std::endl;
		}
	}else{
		m_memory->write(addr, value);
	}
};

unsigned char Gb_bus::read(unsigned short addr){;
	if(addr <= 0x7fff) return m_cart->read_rom(addr);
	else if(addr >= 0xa000 && addr <= 0xbfff) return m_cart->read_ram(addr);
	else if(addr == 0xffff) return m_intrs->IE();
	else if(addr >= 0xff00 && addr <= 0xff7f){
		if(m_rmap.find(addr) != m_rmap.end()){
			return m_rmap[addr]();
		}else{
			//std::cout << "I/O register " << std::hex << addr << " not supported." << std::endl;
		}
	}else{
		return m_memory->read(addr);
	}

	return 0xff;
};
