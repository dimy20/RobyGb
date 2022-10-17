#include "Gb_cartridge.h"

#define GB_DIE(s) do { \
	std::cout << s << std::endl; \
		exit(EXIT_FAILURE); \
}while(0)

Gb_cartridge::Gb_cartridge(const char * filename){
	memset(m_memory, 0, sizeof(BYTE) * CARTRIDGE_MAX_SIZE);
	int size;

	std::ifstream file(filename, std::ios::binary | std::ios::ate);
	size = file.tellg();
	if(size > CARTRIDGE_MAX_SIZE){
		std::cerr << "Cartridge too big." << std::endl;
	}
	file.seekg(0, std::ios::beg);
	file.read((char *)m_memory, size);
	if(!file) std::cerr << "Failed to load cartridge rom" << std::endl;

	// discover mbc
	auto mbc = hw_type();
	switch (mbc)
	{
	   case Gb_cartridge::hardware_type::MBC1: m_mbc1 = true ; break;
	   case Gb_cartridge::hardware_type::MBC1_RAM: m_mbc1 = true ; break;
	   case Gb_cartridge::hardware_type::MBC1_RAM_BATTERY: m_mbc1 = true ; break;
	   case Gb_cartridge::hardware_type::MBC2: m_mbc2 = true ; break;
	   case Gb_cartridge::hardware_type::MBC2_BATTERY: m_mbc2 = true ; break;
	   default : break ;
	}

};

Gb_cartridge::hardware_type Gb_cartridge::hw_type() const { 
	return static_cast<hardware_type>(m_memory[header::HARDWARE_TYPE]);
};

int Gb_cartridge::rom_size() const { return m_memory[header::ROM_SIZE]; };
int Gb_cartridge::ram_size() const { return m_memory[header::RAM_SIZE]; };

// intercept writes to rom and mbc commands
void Gb_cartridge::mbc_intercept(WORD addr, BYTE value){
	assert(addr <= 0x7fff);
	assert(m_mbc1 || m_mbc2);
	// handle ram enable
	if(m_mbc1){
		if(addr < 0x2000){
			mbc.m_ram_enable = (addr & 0xf == 0xa) ? true : false;
			// handle rom change
		}else if(addr >= 0x2000 && addr <= 0x3fff){
			BYTE lo5 = value & 0x1f;
			mbc.m_rom_bank_number &= 0xe0;
			mbc.m_rom_bank_number |= lo5;
			if(mbc.m_rom_bank_number == 0)
				mbc.m_rom_bank_number = 1;
			return;
			// games write here when the m_cart needs a >5 bit bank number, for two
			// additional bits so they can index the higher banks.
		}else if(addr >= 0x4000 && addr <= 0x5fff){
				// mbc1
			if(mbc.m_banking_mode == SIMPLE_BANKING_MODE){
				// rom is <= 512kb
				if(rom_size() <= 0x1f) return;

				mbc.m_rom_bank_number &= 0x1f;
				value &= 0x60; // the two extra bits
				mbc.m_rom_bank_number |= value;
				if(mbc.m_rom_bank_number == 0)
					mbc.m_rom_bank_number = 1;
				return;
			}
			if(mbc.m_banking_mode == RAM_BANKING_MODE){
				// ram is <= 8kb
				if(ram_size() <= 0x0) return;

				mbc.m_ram_bank_number = value & 0x3;
				return;
			};

		// banking mode selection
		}else if(addr >= 0x6000 && addr <= 0x7fff){
			// not large enough, no effect.
			if(ram_size() <= 0x0 && rom_size() <= 0x1f) return;

			if(value & 0x1 == 0){
				mbc.m_banking_mode = SIMPLE_BANKING_MODE;
			    mbc.m_ram_bank_number = 0;
			}else mbc.m_banking_mode = RAM_BANKING_MODE;
		}
	}

};

void Gb_cartridge::write(WORD addr, BYTE value) { 
	assert(addr >= 0xa000 && addr <= 0xbfff && 
								"External ram addr sould be in range 0xa000 - 0xbfff");

	if(!mbc.m_ram_enable) GB_DIE("Writing to disabled external ram.");
	BYTE offset = (addr - 0xa0000);
	m_ram[offset + (mbc.m_ram_bank_number * 0x2000)] = value;
};

BYTE Gb_cartridge::read_rom(WORD addr){
	assert(addr <= 0x8000 && "Rom addr should be in range 0x0000 - 0x8000");
	if(addr <= 0x3fff) return m_memory[addr];
	return m_memory[addr + (mbc.m_rom_bank_number * 0x4000)];
};

BYTE Gb_cartridge::read_ram(WORD addr){
	assert(addr >= 0xa000 && addr <= 0xbfff && 
								"External ram addr sould be in range 0xa000 - 0xbfff");

	if(!mbc.m_ram_enable) GB_DIE("Readig from disabled external ram.");
	BYTE offset = addr - 0xa000;
	// wrong leave it for now
	return m_ram[offset + (mbc.m_ram_bank_number * 0x2000)];
};

