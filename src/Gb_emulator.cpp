#include "Gb_emulator.h"

Gb_emulator::Gb_emulator(){};

void Gb_emulator::run(const char * filename){
	Gb_cartridge cartridge(filename);
	m_mmu.init(&cartridge);

	void * label_addr = &&label;

	goto *label_addr;

	label:
		std::cout << "this is a label" << std::endl;
	while(1){ 

	};
};
