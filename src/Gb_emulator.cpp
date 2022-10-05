#include "Gb_emulator.h"

Gb_emulator::Gb_emulator(){};

void Gb_emulator::run(const char * filename){
	Gb_cartridge cartridge(filename);
	m_mmu.init(&cartridge);
	m_core = Gb_core(&m_mmu);
	m_core.init();
	m_core.emulate_cycles(4); // dummy for now
};

