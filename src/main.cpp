#include <iostream>
#include "Gb_emulator.h"
#include "Gb_cartridge.h"
int main(){
	std::cout << "Gameboy Emulator!" << std::endl;
	Gb_emulator gb;
	gb.run("testgb.gb");
	return 0;
};
