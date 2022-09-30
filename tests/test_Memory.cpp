#include <gtest/gtest.h>
#include <cstdlib>
#include "Memory.h"

TEST(Memory, wram_mirror){
	srand(time(NULL));
	Mem_mu memory;
	Gb_cartridge cart("../contra.gb");
	memory.init(&cart);

	const WORD start = 0xe000;

	for(int i = 0; i < 50; i++){
		BYTE x = rand() % 255;
		memory.write(start + i, x);
		ASSERT_EQ(memory.read(0xc000 + i), x);
	}

	memory.write(0xc000 + 100, 20);
	// reading from echo wram should be the same as reading from the corresponding
	// wram bank
	ASSERT_EQ(memory.read(0xe000 + 100), memory.read(0xc000 + 100));

	// test writing at end of mirror wram
	memory.write(0xfdff, 1);
	ASSERT_EQ(memory.read(0xfdff), memory.read(0xddff));
	// test some random address in the range 0xc000 - 0xddff
	WORD random = 0xc000 + rand() % ((0xddff + 1) - 0xc000);
	BYTE val = rand() % 0xff;
	memory.write(random, val);
	ASSERT_EQ(memory.read(random), memory.read(random + 0x2000));
};

TEST(Memory, mbc1_registers){
	Mem_mu memory;
	Gb_cartridge cart("../contra.gb");
	memory.init(&cart);

	// ram enable mbc1 register
	EXPECT_EXIT(memory.read(0xa000), testing::ExitedWithCode(1), ".*");
	EXPECT_EXIT(memory.write(0xa000, 0xff), testing::ExitedWithCode(1), ".*");
	memory.write(0x0000, 0xa); // enable external ram
	memory.write(0xa000, 0xff); // now can write to external ram
	EXPECT_EQ(memory.read(0xa000), 0xff);
	memory.write(0x0000, 0x0); // desable external ram
	EXPECT_EXIT(memory.read(0xa000), testing::ExitedWithCode(1), ".*");

	memory.write(0x0000, 0x1); // desable external ram
	EXPECT_EXIT(memory.read(0xa000), testing::ExitedWithCode(1), ".*");

	// rom bank number mbc1 register
	int rom_banks = cart.rom_size();
	for(int i = 0; i < rom_banks; i++){
		memory.write(0x2000, (BYTE)i); // desable external ram
		if(i == 0)
			EXPECT_EQ(cart.mbc.m_rom_bank_number, 1);
		else
			EXPECT_EQ(cart.mbc.m_rom_bank_number, i);
	};
};

int main(int argc, char ** argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
