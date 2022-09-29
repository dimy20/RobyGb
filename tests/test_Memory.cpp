#include <gtest/gtest.h>
#include <cstdlib>
#include "Memory.h"

TEST(Memory, wram_mirror){
	srand(time(NULL));
	Mem_mu memory;
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

int main(int argc, char ** argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
