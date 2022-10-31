#include "Gb_timer.h"
#include "Memory.h"

void Gb_timer::init(Mem_mu * memory){
	m_memory = memory;
	m_frequency = 4096; // default timer frequency
	m_divider_counter = 256;
	m_timer_counter = CLOCK_SPEED / m_frequency;
};

void Gb_timer::update(int cycles){
	update_divider(cycles);

	if(!enabled()) return;

	m_timer_counter -= cycles;

	if(m_timer_counter <= 0){
		BYTE timer_counter = m_memory->read(TIMA);

		if(timer_counter == 0xff){
			// reset
			BYTE timer_modulo = m_memory->read(TMA);
			m_memory->write(TIMA, timer_modulo);
			// interrupt
			send_interrupt(2);
		}else{
			m_memory->write(TIMA, timer_counter + 1);
		}
	};
};

void Gb_timer::send_interrupt(int i){
	auto IF = m_memory->read(0xff0f);
	IF |= (0x1 << i);
	m_memory->write(0xff0f, IF);
};


bool Gb_timer::enabled(){
	BYTE timer_control = m_memory->read(0xff07);
	return (timer_control >> 2);
};

void Gb_timer::update_frequency(){
	BYTE freq = m_memory->read(0xff07) & 0x3;
	switch (freq){
		case 0: m_frequency = 4096;   break ;
		case 1: m_frequency = 262144; break ;
		case 2: m_frequency = 65536;  break ;
		case 3: m_frequency = 16382;  break ;
	}
	m_timer_counter = CLOCK_SPEED / m_frequency;
};

void Gb_timer::update_divider(int cycles){
	auto divider_reg = m_memory->m_memory.io_registers[0xff04 - 0xff00];
	m_divider_counter -= cycles;
	if(m_divider_counter <= 0){
		divider_reg = divider_reg == 0xff ? 0 : divider_reg + 1;
		m_divider_counter = 256;
		m_memory->m_memory.io_registers[0xff04 - 0xff00] = divider_reg;
	};
};
