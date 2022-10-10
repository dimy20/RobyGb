#include <gtest/gtest.h>
#include "Gb_core.h"
#include "Gb_cartridge.h"
#include "Memory.h"

TEST(Core, ld_8bit_u8){
	Gb_cartridge cart("../../tests/test_roms/test_rom_8bit_loads.gb");
	Mem_mu memory;
	memory.init(&cart);
	Gb_core core(&memory);
	core.init();
	core.emulate_cycles(8);

	for(int r1 = 0; r1 < 8; r1++){
		if(r1 == 6) continue;
		BYTE reg1;

		for(int r2 = 0; r2 < 7; r2++){
			core.emulate_cycles(1);
			reg1 = core.r_X(static_cast<Gb_core::reg_order>(r1));

			switch(static_cast<Gb_core::reg_order>(r2)){
				case Gb_core::reg_order::REG_B: ASSERT_EQ(reg1, core.r_BC().hi); break;
				case Gb_core::reg_order::REG_C: ASSERT_EQ(reg1, core.r_BC().lo); break;
				case Gb_core::reg_order::REG_D: ASSERT_EQ(reg1, core.r_DE().hi); break;
				case Gb_core::reg_order::REG_E: ASSERT_EQ(reg1, core.r_DE().lo); break;
				case Gb_core::reg_order::REG_H: ASSERT_EQ(reg1, core.r_HL().hi); break;
				case Gb_core::reg_order::REG_L: ASSERT_EQ(reg1, core.r_HL().lo); break;
				case Gb_core::reg_order::REG_A: ASSERT_EQ(reg1, core.r_AF().hi); break;
				case Gb_core::reg_order::REG_HL: continue;
			};

		};
	}

	// test : ld[xx], a
	core.emulate_cycles(11);
	ASSERT_EQ(memory.read(0xc000), 1);
	ASSERT_EQ(memory.read(0xc001), 255);
	ASSERT_EQ(memory.read(0xc002), 255);
	ASSERT_EQ(memory.read(0xc003), 255);
	ASSERT_EQ(memory.read(0xc005), 10);
}

TEST(Core, ld_8bit_01){
	Gb_cartridge cart("../../tests/test_roms/8bit_ld/test_rom_01.gb");
	Mem_mu memory;
	memory.init(&cart);
	Gb_core core(&memory);
	core.init();
	core.emulate_cycles(1);

	core.emulate_cycles(5);
	ASSERT_EQ(core.r_AF().hi, 10);
	core.emulate_cycles(5);
	ASSERT_EQ(core.r_AF().hi, 100);
	core.emulate_cycles(3);
	ASSERT_EQ(core.r_AF().hi, 128);
	core.emulate_cycles(3);
	ASSERT_EQ(core.r_AF().hi, 234);
	core.emulate_cycles(2);
	ASSERT_EQ(memory.read(0xc005), 255);
	core.emulate_cycles(1);
	ASSERT_EQ(core.r_AF().hi, 128);
	core.emulate_cycles(3);
	ASSERT_EQ(memory.read(0xff00), 10);
	core.emulate_cycles(1);
	ASSERT_EQ(memory.read(0xff00 + 10), 10);
	core.emulate_cycles(2);
	ASSERT_EQ(core.r_AF().hi, 10);
	core.emulate_cycles(3);
	ASSERT_EQ(memory.read(0xff00 + core.r_BC().lo), 1);
	core.emulate_cycles(2);
	ASSERT_EQ(core.r_AF().hi, 1);
};

TEST(Core, ld_16bit){
	Gb_cartridge cart("../../tests/test_roms/16bit_ld/test_rom.gb");
	Mem_mu memory;
	memory.init(&cart);
	Gb_core core(&memory);
	core.init();
	core.emulate_cycles(1 + 4);

	ASSERT_EQ(core.r_BC().pair, 0xc000);
	ASSERT_EQ(core.r_DE().pair, 0xc001);
	ASSERT_EQ(core.r_HL().pair, 0xc002);
	ASSERT_EQ(core.sp().pair, 0xfffe);
}

TEST(Core, ld_16bit_sp){
	Gb_cartridge cart("../../tests/test_roms/16bit_ld/test_rom_sp.gb");
	Mem_mu memory;
	memory.init(&cart);
	Gb_core core(&memory);
	core.init();
	core.emulate_cycles(1);

	core.emulate_cycles(2);

	ASSERT_EQ(core.r_BC().pair, 1024);
	auto v = (memory.read(core.sp().pair + 2) << 8) | memory.read(core.sp().pair + 1);
	//ASSERT_EQ(v, 1024);
	core.emulate_cycles(2);
	ASSERT_EQ(core.r_BC().pair, 1024);

	core.emulate_cycles(2);
	v = (memory.read(core.sp().pair + 2) << 8) | memory.read(core.sp().pair + 1);
	ASSERT_EQ(v, 1024);
	core.emulate_cycles(2);
	ASSERT_EQ(core.r_DE().pair, 1024);

	core.emulate_cycles(2);
	v = (memory.read(core.sp().pair + 2) << 8) | memory.read(core.sp().pair + 1);
	ASSERT_EQ(v, 1024);
	core.emulate_cycles(2);
	ASSERT_EQ(core.r_HL().pair, 1024);

	core.emulate_cycles(2);
	ASSERT_EQ(20, memory.read(core.sp().pair + 2));
	core.emulate_cycles(1);
	ASSERT_EQ(core.r_AF().hi, 20);
}

TEST(Core, alu){
	Gb_cartridge cart("../../tests/test_roms/alu/alu1.gb");
	Mem_mu memory;
	memory.init(&cart);
	Gb_core core(&memory);
	core.init();

	core.emulate_cycles(1);
	core.emulate_cycles(7);

	int sum = 0;
	for(int i = 1; i <= 6; i++){
		sum += i;
		core.emulate_cycles(1);
		ASSERT_EQ(core.r_AF().hi, sum);
	}
	core.emulate_cycles(3);
	ASSERT_EQ(core.r_AF().hi, 28);
	core.emulate_cycles(1);
	ASSERT_EQ(core.r_AF().hi, 28 * 2);
};

int main(int argc, char ** argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
