#include <iostream>
#include "Gb_emulator.h"
#include "Gb_cartridge.h"
int main(){
	Gb_emulator gb;
	gb.run("01-special.gb");
	return 0;
};
