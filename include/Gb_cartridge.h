#pragma once
#include <cstring>
#include <iostream>
#include <fstream>
#include <cassert>
#include "Gb_types.h"

#define CARTRIDGE_MAX_SIZE 1024 * 1024 * 2
#define SIMPLE_BANKING_MODE 0x0
#define RAM_BANKING_MODE 0x1

class Gb_cartridge{
	public:
		// offsets in the cartridge header.
		enum header{
			HARDWARE_TYPE = 0x0147,
			ROM_SIZE = 0x0148,
			RAM_SIZE = 0x0149
		};

		enum hardware_type{
			MBC1 = 0x1,
			MBC1_RAM = 0x2,
			MBC1_RAM_BATTERY = 0x3,
			MBC2 = 0x5,
			MBC2_BATTERY = 0x6
		};

		Gb_cartridge(const char * filename);
		operator bool() const;
		hardware_type hw_type() const;
		int ram_size() const; // number of ram banks
		int rom_size() const; // number of rom banks
		/* reads from rom bank0 if addr <= 0x3fff or from the currently selected
		 * rom bank mapped to the 0x4000 - 0x7fff range.*/
		BYTE read_rom(WORD addr);
		/* reads from external ram in cartridge of min size 8kb max size 32kb, 
		 * depending on the currently selected ram bank mapped to the 0xa000 - 0xbfff
		 * range, to perform this operation RAM must be enabled first.*/
		BYTE read_ram(WORD addr);
		/* writes to external ram in cartridge of min size 8kb max size 32kb, 
		 * depending on the currently selected ram bank mapped to the 0xa000 - 0xbfff 
		 * range, to perfom this operation RAM must be enabled first.*/
		void write(WORD addr, BYTE value);
		/* Intercepts any writes to the range 0x0000-0x7fff (READ ONLY) and intercepts
		 * the values according to the mbc type present on the cartridge.*/
		void mbc_intercept(WORD addr, BYTE value);
		void mbc1(unsigned short addr, unsigned char value);
	private:
		BYTE m_memory[CARTRIDGE_MAX_SIZE];
		BYTE m_ram[0x8000]; // max ram, 32kb max
		void load_rom(const char * filename);
		bool m_mbc1 = false;
		bool m_mbc2 = false;
		//unsigned char m_rombank_num;
		//unsigned char m_rmabank_num;



		bool m_ram_enabled;
		int m_rom_banking_mode = 0;
		int m_rombank_num = 1;
		int m_rambank_num = 0;

};


