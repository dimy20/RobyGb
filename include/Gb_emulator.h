#pragma once

#include <iostream>
#include "Memory.h"
#include "Gb_cartridge.h"
#include "Gb_core.h"

class Gb_emulator{
	public:
		Gb_emulator();
		void run(const char * filename);
	private:
		Mem_mu m_mmu;
		Gb_core m_core;
};

