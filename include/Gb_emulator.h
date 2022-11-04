#pragma once

#include <iostream>
#include "Memory.h"
#include "Gb_cartridge.h"
#include "Gb_core.h"
#include "Gb_ppu.h"
#include "Gb_timer.h"
#include "Window.h"

#include <unistd.h>
class Gb_emulator{
	public:
		Gb_emulator();
		void run(const char * filename);
	private:
		void signal_interrupt(Gb_core::intrp i);
		// updates system's timer, takes how many cycles have passed
		// so we can synchronize it with the cpu.
		void handle_timer(int cycles);
	private:
		Mem_mu m_memory;
		Gb_core m_core;
		Gb_ppu m_ppu;
		Window m_window;
		Gb_timer m_timer;
};

