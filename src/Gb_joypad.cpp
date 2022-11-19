#include "Gb_joypad.h"

void Gb_joypad::init(Gb_interrupts * intrs){  m_intrs = intrs; };

void Gb_joypad::set_directions(unsigned char value){
	unsigned char old = m_directions & 0xf;
	if(!m_directions_selected)
		update_IF(old, value);
	m_directions = value & 0xf; 
}


void Gb_joypad::set_buttons(unsigned char value){
	unsigned char old = m_buttons & 0xf;
	if(!m_buttons_selected)
		update_IF(old, value & 0xf);
	m_buttons = value & 0xf; 
}

void Gb_joypad::update_IF(unsigned char old_line, unsigned char new_line){
	if(old_line == new_line) return;
	for(int i = 0; i < 4; i++){
		unsigned char old_bit = (old_line >> i) & 1;
		unsigned char new_bit = (new_line >> i) & 1;
		if(!old_bit && new_bit){
			m_intrs->request(4);
		}

	}
};

unsigned char Gb_joypad::P1(){
	unsigned char mask = 0;
	if(!m_directions_selected) mask = m_directions;
	if(!m_buttons_selected ) mask = m_buttons;

	unsigned char selected_line = mask ^ 0xf; // flip bits, Selected = 0

	return 0xf0 /*| m_directions_selected | m_buttons_selected*/ | selected_line;
};

void Gb_joypad::set_P1(unsigned char value){ 
	m_directions_selected = (value & 0x10); // if selected bit is set to 0.
	m_buttons_selected = (value & 0x20);
};
