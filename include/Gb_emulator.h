#pragma once

#include <iostream>
#include "Memory.h"
#include "Gb_cartridge.h"
#include "Gb_core.h"
#include "Gb_ppu.h"
#include "Gb_timer.h"
#include "Gb_interrupts.h"
#include "Window.h"
#include "Gb_bus.h"
#include "Gb_joypad.h"

#include <unistd.h>
class Gb_emulator{
	public:
		Gb_emulator();
		void run(const char * filename);
		void init();
	private:
		Mem_mu m_memory;
		Gb_core m_core;
		Gb_ppu m_ppu;
		Window m_window;
		Gb_timer m_timer;
		Gb_interrupts m_intrs;
		Gb_bus m_bus;
		Gb_joypad m_joypad;
};

