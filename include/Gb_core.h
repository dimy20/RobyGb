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

typedef struct opcode_s opcode_t;


struct Gb_register{
	WORD pair; /* 16 bit paired register ex. HL*/
	struct{
		BYTE lo; // low byte ex. L
		BYTE hi; // high byte ex. H
	};
};

class Gb_core{
	friend opcode_t;

	public:
		enum reg_order{
			REG_B, REG_C,
			REG_D, REG_E,
			REG_H, REG_L,
			REG_HL, REG_A
		};
		enum class Is;

	enum class Is;
	public:
		Gb_core(Mem_mu * memory);
		void emulate_cycles();
	private:
		void dispatch_next(); // disptach next instruction
		int _8bit_load(BYTE& rg, BYTE value);
		int _8bit_ld_r1r2();
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

		std::map<Is, void *> m_dispatch_table;
};

enum class Gb_core::Is{
	// ld r,r : r1 = r2
	LD_8BIT_AA = 0x7f,
	LD_8BIT_AB = 0X78,
	LD_8BIT_AC = 0x79,
	LD_8BIT_AD = 0x7a,
	LD_8BIT_AE = 0x7b,
	LD_8BIT_AH = 0x7c,
	LD_8BIT_AL = 0x7d,
	LD_8BIT_AHL = 0x7e,
	// ld B, V
	LD_8BIT_BB = 0x40,
	LD_8BIT_BC = 0x41,
	LD_8BIT_BD = 0x42,
	LD_8BIT_BE = 0x43,
	LD_8BIT_BH = 0x44,
	LD_8BIT_BL = 0x45,
	LD_8BIT_BHL = 0x46,
	LD_8BIT_BA = 0x47,
	// LD C, V
	LD_8BIT_CB = 0x48,
	LD_8BIT_CC = 0x49,
	LD_8BIT_CD = 0x4a,
	LD_8BIT_CE = 0x4b,
	LD_8BIT_CH = 0x4c,
	LD_8BIT_CL = 0x4d,
	LD_8BIT_CHL = 0x4e,
	LD_8BIT_CA = 0x4f,
	// LD D, V
	LD_8BIT_DB = 0x50,
	LD_8BIT_DC = 0x51,
	LD_8BIT_DD = 0x52,
	LD_8BIT_DE = 0x53,
	LD_8BIT_DH = 0x54,
	LD_8BIT_DL = 0x55,
	LD_8BIT_DHL = 0x56,
	LD_8BIT_DA = 0x57,
	// LD E, V
	LD_8BIT_EB = 0x58,
	LD_8BIT_EC = 0x59,
	LD_8BIT_ED = 0x5a,
	LD_8BIT_EE = 0x5b,
	LD_8BIT_EH = 0x5c,
	LD_8BIT_EL = 0x5d,
	LD_8BIT_EHL = 0x5e,
	LD_8BIT_EA = 0x5f,
	// LD H, V
	LD_8BIT_HB = 0x60,
	LD_8BIT_HC = 0x61,
	LD_8BIT_HD = 0x62,
	LD_8BIT_HE = 0x63,
	LD_8BIT_HH = 0x64,
	LD_8BIT_HL = 0x65,
	LD_8BIT_HHL = 0x66,
	LD_8BIT_HA = 0x67,
	// LD H, V
	LD_8BIT_LB = 0x68,
	LD_8BIT_LC = 0x69,
	LD_8BIT_LD = 0x6a,
	LD_8BIT_LE = 0x6b,
	LD_8BIT_LH = 0x6c,
	LD_8BIT_LL = 0x6d,
	LD_8BIT_LHL = 0x6e,
	LD_8BIT_LA = 0x6f,
	//
	LD_8BIT_HLB = 0x70,
	LD_8BIT_HLC = 0x71,
	LD_8BIT_HLD = 0x72,
	LD_8BIT_HLE = 0x73,
	LD_8BIT_HLH = 0x74,
	LD_8BIT_HLL = 0x75,
	LD_8BIT_HLA = 0x77,
	// ld r, u8
	LD_B_U8 = 0x06,
	LD_D_U8 = 0x16,
	LD_H_U8 = 0x26,
	LD_HL_U8 = 0x36,
	LD_C_U8 = 0x0e,
	LD_E_U8 = 0x1e,
	LD_L_U8 = 0x2e,
	LD_A_U8 = 0x3e,
		

	// jpm calls
	JMP_NN = 0xc3
};

typedef struct opcode_s{
	opcode_s(Gb_core::Is op, void(Gb_core::* f)(void), int c) : opcode(op), fn(f), cycles(c){};
	public:
		Gb_core::Is opcode;
		void (Gb_core::* fn)(void);
		int cycles;
}opcode_t;
