#include "Gb_emulator.h"

Gb_emulator::Gb_emulator(){};

void Gb_emulator::run(const char * filename){
	Gb_cartridge cartridge(filename);
	m_mmu.init(&cartridge);
};
