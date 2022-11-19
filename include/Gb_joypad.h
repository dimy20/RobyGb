#pragma once
#include <iostream>
#include "Gb_types.h"
#include "Gb_interrupts.h"

class Window;
class Gb_joypad{
	public:
		friend class Window;
		Gb_joypad() = default;
		void set_P1(unsigned char value);
		unsigned char P1();
		void init(Gb_interrupts * intrs);
		void set_directions(unsigned char value);
		void set_buttons(unsigned char value);
	private:
		void update_IF(unsigned char old, unsigned char new_value);
	private:
		Gb_interrupts * m_intrs;
		unsigned char m_directions_selected = 0x10;
		unsigned char m_buttons_selected = 0x20;
		unsigned char m_directions = 0;
		unsigned char m_buttons = 0;

};
