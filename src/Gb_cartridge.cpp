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
	int mbc = m_memory[0x147];
	assert(addr <= 0x7fff);
	if(mbc == 0) return;
	else if(m_mbc1)
		mbc1(addr, value);
	else{
		GB_DIE("Unsuported mbc " << mbc);
	}
};

void Gb_cartridge::mbc1(unsigned short addr, unsigned char value){
	if(addr <= 0x1fff){ // ram enable register
		m_ram_enabled = (value & 0xf) == 0xa;
	}else if (addr >= 0x2000 && addr <= 0x3fff){ // rom bank number register
		// this register selects the rom bank number for the $4000 - $7fff region
		int num = value & 0x1f;
		num = num == 0 ? 1 : num;
		m_rombank_num = (m_rombank_num & 0xe0) | num;
		if(!m_rom_banking_mode)
			m_rombank_num &= 0x6f; // clear 2 upper bits

		if(num == 0 || num == 0x20 || num == 0x40 || num == 0x60) num++;

	}else if(addr >= 0x4000 && addr <= 0x5fff){
		value &= 0x3;
		if(m_rom_banking_mode){
			m_rombank_num |= value << 5; // 2 upper bits
		}else{
			m_rambank_num = value;
		}
	}else if(addr >= 0x6000 && addr <= 0x7fff){
		value &= 1;
		if(value == 0){ // ROM banking mode enabled
			m_rambank_num = 0;
		}
		m_rom_banking_mode = value;
	}

};

/*
void Gb_cartridge::mbc5(unsigned short addr, unsigned char value){

};
*/


void Gb_cartridge::write(WORD addr, BYTE value) { 
	assert(addr >= 0xa000 && addr <= 0xbfff && 
								"External ram addr sould be in range 0xa000 - 0xbfff");

	if(!m_ram_enabled) GB_DIE("Writing to disabled external ram.");
	BYTE offset = (addr - 0xa0000);
	m_ram[offset + (m_rambank_num * 0x2000)] = value;
};

BYTE Gb_cartridge::read_rom(WORD addr){
	assert(addr <= 0x8000 && "Rom addr should be in range 0x0000 - 0x8000");
	if(addr <= 0x3fff) return m_memory[addr];
	unsigned short bank_offset = (m_rombank_num - 1) * 0x4000;
	assert(bank_offset + addr < CARTRIDGE_MAX_SIZE);
	return m_memory[bank_offset + addr];
};

BYTE Gb_cartridge::read_ram(WORD addr){
	assert(addr >= 0xa000 && addr <= 0xbfff && 
								"External ram addr sould be in range 0xa000 - 0xbfff");

	//if(!mbc.m_ram_enable) GB_DIE("Readig from disabled external ram.");
	BYTE offset = addr - 0xa000;
	return m_ram[offset + (m_rambank_num * 0x2000)];
};

