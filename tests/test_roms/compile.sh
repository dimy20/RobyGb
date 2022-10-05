#!/bin/sh
rgbasm -L -o test_rom_8bit_loads.o test_rom_8bit_loads.asm
rgblink -o test_rom_8bit_loads.gb test_rom_8bit_loads.o
rgbfix -v -p 0xFF test_rom_8bit_loads.gb
