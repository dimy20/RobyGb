#include "Gb_emulator.h"

Gb_emulator::Gb_emulator(){
	m_window.init();
};

void Gb_emulator::run(const char * filename){
	Gb_cartridge cartridge(filename);
	m_timer.init(&m_memory);
	m_memory.init(&cartridge, &m_timer);
	m_core = Gb_core(&m_memory);
	m_core.init();
	m_ppu.init(&m_memory, &m_window);
	int max_cycles_per_frame = 69905;
	int cycles = 0;
	while(!m_window.should_close()){
		cycles = 0;
		while(cycles < max_cycles_per_frame){
			cycles += m_core.execute_instruction();
			m_ppu.update_graphics(cycles);
			m_timer.update(cycles);
			m_core.handle_interrupts();

		};

		m_window.draw();
		m_window.swap_buffers();

	};
	std::cout << "Executed instructions ..." << std::endl;
};

void Gb_emulator::signal_interrupt(Gb_core::intrp i){
	auto IF = m_memory.read(0xff0f);
	IF |= (0x1 << i);
	m_memory.write(0xff0f, IF);
};


