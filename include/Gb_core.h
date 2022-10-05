#pragma once
#include <map>
#include <memory>
#include <variant>
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

struct Gb_instruction;
struct Gb_register{
	WORD pair; /* 16 bit paired register ex. HL*/
	struct{
		BYTE lo; // low byte ex. L
		BYTE hi; // high byte ex. H
	};
};

class Gb_core{
	friend Gb_instruction;

	public:
		enum reg_order{
			REG_B, REG_C,
			REG_D, REG_E,
			REG_H, REG_L,
			REG_HL, REG_A
		};
		enum class ld_8bit;
		enum class i_control;

	public:
		Gb_core(Mem_mu * memory);
		void emulate_cycles(int n);
		void init();

	private:

		int _8bit_load(BYTE& rg, BYTE value);
		void _8bit_ld_r1r2();
		int ld_r_v(BYTE& r, BYTE v);
		//Load absolute address addr with data from the 8-bit register r.
		int ld_addr_r(WORD addr, BYTE r);
		void jmp_nn();


		void _8bit_ldu8();
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

		// table for 8 bit load operations.
		std::map<ld_8bit, std::shared_ptr<Gb_instruction>> m_8bit_load_table;
		// table for jump calls
		std::map<i_control, std::shared_ptr<Gb_instruction>> m_jmp_table;
		// table for 8-bit register loads of 8-bit inmediate data.
		std::map<ld_8bit, std::shared_ptr<Gb_instruction>> m_8bit_ldu8_table;

};

enum class Gb_core::ld_8bit{
	// ld r,r : r1 = r2
	A_A = 0x7f, B_B = 0x40, C_B = 0x48, D_B = 0x50, E_B = 0x58, H_B = 0x60, L_B = 0x68,
	A_B = 0X78, B_C = 0x41, C_C = 0x49, D_C = 0x51, E_C = 0x59, H_C = 0x61, L_C = 0x69,
	A_C = 0x79, B_D = 0x42, C_D = 0x4a, D_D = 0x52, E_D = 0x5a, H_D = 0x62, L_D = 0x6a,
	A_D = 0x7a, B_E = 0x43, C_E = 0x4b, D_E = 0x53, E_E = 0x5b, H_E = 0x63, L_E = 0x6b,
	A_E = 0x7b, B_H = 0x44, C_H = 0x4c, D_H = 0x54, E_H = 0x5c, H_H = 0x64, L_H = 0x6c,
	A_H = 0x7c, B_L = 0x45, C_L = 0x4d, D_L = 0x55, E_L = 0x5d, H_L = 0x65, L_L = 0x6d,
	A_L = 0x7d, B_HL = 0x46,C_HL = 0x4e,D_HL = 0x56,E_HL = 0x5e,H_HL = 0x66,L_HL = 0x6e,
	A_HL = 0x7e,B_A = 0x47, C_A = 0x4f, D_A = 0x57, E_A = 0x5f, H_A = 0x67, L_A = 0x6f,
				  // ld r, u8
	HL_B = 0x70,  B_U8 = 0x06, A_U8 = 0x3e,
	HL_C = 0x71,  D_U8 = 0x16,
	HL_D = 0x72,  H_U8 = 0x26,
	HL_E = 0x73,  HL_U8 = 0x36,
	HL_H = 0x74,  C_U8 = 0x0e,
	HL_L = 0x75,  E_U8 = 0x1e,
	HL_A = 0x77,  L_U8 = 0x2e,
};

enum class Gb_core::i_control{
	JMP_NN = 0xc3
};

struct Gb_instruction{
	Gb_instruction(std::variant<Gb_core::ld_8bit, Gb_core::i_control> op, void(Gb_core::* f)(void), int c) : opcode(op), fn(f), cycles(c){};
	public:
		std::variant<Gb_core::ld_8bit, Gb_core::i_control> opcode;
		//Gb_core::Is opcode;
		void (Gb_core::* fn)(void);
		int cycles;
};
