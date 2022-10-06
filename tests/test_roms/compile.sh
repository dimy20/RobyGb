#!/bin/sh
rgbasm -L -o test_rom_8bit_loads.o test_rom_8bit_loads.asm
rgblink -o test_rom_8bit_loads.gb test_rom_8bit_loads.o
rgbfix -v -p 0xFF test_rom_8bit_loads.gb

rgbasm -L -o 16bit_ld/test_rom.o 16bit_ld/test_rom.asm
rgblink -o 16bit_ld/test_rom.gb 16bit_ld/test_rom.o
rgbfix -v -p 0xFF 16bit_ld/test_rom.gb
