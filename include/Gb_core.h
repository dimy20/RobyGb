#pragma once
#include <map>
#include "Gb_types.h"
#include "Memory.h"
/* Screen */
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_DEPTH 3

#define ROM_SIZE 0x200000
#define STACK_SIZE 16
#define ENTRY_POINT 0x0100 // after displaying the nintendo logo, boot rom jumps here.
#define SP_INIT_ADDR 0xfffe

/* flags */
#define ZERO_FLAG 7 
#define SUBSTRACT_FLAG 6
#define HALF_CARRY_FLAG 5
#define CARRY_FLAG 4

/* BC -> ------- -------- 
 *	     low C    high B 
 **/

#define LD_8BIT_CYCLES 8
struct Gb_register{
	WORD pair; /* 16 bit paired register ex. HL*/
	struct{
		BYTE lo; // low byte ex. L
		BYTE hi; // high byte ex. H
	};
};

class Gb_core{
	enum class Is{
		LD_8BIT_B = 0x6,
		LD_8BIT_C = 0xe,
		LD_8BIT_D = 0x16,
		LD_8BIT_E = 0x1e,
		LD_8BIT_H = 0x26,
		LD_8BIT_L = 0x2e
	};

	public:
		Gb_core(Mem_mu * memory);
		void emulate_cycles();
	private:
		void dispatch_next(); // disptach next instruction
		int _8bit_load(BYTE& rg);
	private:
		Gb_register m_registerAF; 
		Gb_register m_registerBC;
		Gb_register m_registerDE;
		Gb_register m_registerHL;
		// some instructions acess the low and high bytes of the stack pointer,
		// so we'll represent it as a register.
		WORD m_sp = SP_INIT_ADDR; // high ram
		WORD m_pc = ENTRY_POINT;
		Mem_mu * m_memory;

		std::map<Is, void *> m_dispatch_table;
};
