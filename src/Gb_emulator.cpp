#include "Gb_emulator.h"
#include <chrono>

Gb_emulator::Gb_emulator(){
	m_window.init(&m_joypad);
};

void Gb_emulator::run(const char * filename){
	Gb_cartridge cartridge(filename);
	m_timer.init(&m_intrs);
	m_ppu.init(&m_memory, &m_window, &m_intrs);
	m_bus.init(&m_memory, &m_ppu, &m_timer, &m_intrs, m_ppu.lcd(), &cartridge, &m_joypad);
	m_memory.init(&m_bus);
	m_core.init(&m_intrs, &m_bus);
	m_joypad.init(&m_intrs);


	int max_cycles_per_frame = 69905; // max clocks per frame <- is a more appropiete name.
	int cycles = 0;

	/*
	long one_second = 1000000000;
	auto last_time = std::chrono::high_resolution_clock::now();
	*/

	while(!m_window.should_close()){
		cycles = 0;

		while(cycles < max_cycles_per_frame){
			cycles += m_core.execute_instruction();
			m_ppu.update_graphics(cycles);
			m_core.handle_interrupts();
			m_timer.cycle(cycles);
			m_memory.update_DMA(cycles);
		};

		/*
		auto now = std::chrono::high_resolution_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - last_time).count();
		*/

		
		/*
		if(elapsed >= one_second){
		}
		*/

		m_window.draw();
		m_window.swap_buffers();

	};
	std::cout << "Executed instructions ..." << std::endl;
};
