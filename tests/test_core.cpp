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
	core.emulate_cycles(9);
	ASSERT_EQ(memory.read(0xc000), 1);
	ASSERT_EQ(memory.read(0xc001), 255);
	ASSERT_EQ(memory.read(0xc002), 255);
	ASSERT_EQ(memory.read(0xc003), 255);
}

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

int main(int argc, char ** argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
};
