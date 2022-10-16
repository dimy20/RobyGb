#pragma once
#include <map>
#include <memory>
#include <variant>
#include <vector>
#include "Gb_types.h"
#include "Memory.h"
#include <functional>
/* Screen */
#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
#define SCREEN_DEPTH 3

#define ROM_SIZE 0x200000
#define STACK_SIZE 16
#define ENTRY_POINT 0x0100 // after displaying the nintendo logo, boot rom jumps here.
#define SP_INIT_ADDR 0xfffe
#define IE_ADDR 0xffff

#define ROW(opcode) (static_cast<BYTE>(opcode) & 0xf0) >> 4
#define COL(opcode) (static_cast<BYTE>(opcode) & 0x0f)

#define LD_8BIT_CYCLES 8

class Gb_core{
	public:
		enum reg_order{
			REG_B, REG_C,
			REG_D, REG_E,
			REG_H, REG_L,
			REG_HL, REG_A
		};
		enum flag{
			ZERO = 7,
			SUBS = 6,
			HALF_CARRY = 5,
			CARRY = 4
		};
		enum class ld_8bit;
		enum class ld_16bit;
		enum class i_control;
		enum class alu;

	public:
		Gb_core() = default;
		Gb_core(Mem_mu * memory);
		void emulate_cycles(int n);
		void init();

	private:
		void build_opcode_matrix();
		void build_registers_rmap();
		void build_registers_wmap();
		void build_alu_x80_xbf();
		void build_alu_x04_x45();
		void build_alu_inc_dec();
		void build_control();
		void init_registers();

		int _8bit_load(BYTE& rg, BYTE value);
		//Load absolute address addr with data from the 8-bit register r.
		int ld_addr_r(WORD addr, BYTE r);
		void _8bit_ldu8();
		void _8bit_ld_xxA();
		void _8bit_ld_Axx();
		void _8bit_ld_ff00();

		void _16_bit_ld();

		void jmp_nn();

		void set_flag(flag f, bool set);
		constexpr BYTE get_flag(flag f);

		// alu handlers
		void x8_alu_xor(BYTE r2);
		void x8_alu_add(BYTE r2);
		void x8_alu_adc(BYTE r2);
		void x8_alu_sub(BYTE r2);
		void x8_alu_sbc(BYTE r2);
		void x8_alu_and(BYTE r2);
		void x8_alu_or(BYTE r2);
		void x8_alu_cp(BYTE r2);
		void x8_alu_inc(reg_order n);
		void x8_alu_dec(reg_order n);
		void x8_alu_daa(); // decimal adjust A, after arithmetic instruction
		void x8_alu_cpl(); 
		void x8_alu_ccf(); 
		void x8_alu_scf(); 

		void x16_alu_inc(WORD& rr);
		void x16_alu_dec(WORD& rr);
		void x16_alu_add(WORD rr);
		void x16_alu_addsp();


		// control handlers
		void ctrl_return();
		void ctrl_call();
		void ctrl_jr(bool cond);
		void ctrl_rst(const WORD offset);

		std::vector<ld_8bit> opcodes_8bitld_u8() const;
		std::vector<ld_8bit> opcodes_8bitld_XX_R() const;
		std::vector<ld_8bit> opcodes_8bitld_Axx() const;
		std::vector<ld_8bit> opcodes_ff00() const;

		std::vector<ld_16bit> opcodes_16bitld_u16() const;
		std::vector<ld_16bit> opcodes_16bitld_stack() const;

		constexpr BYTE row(BYTE opcode){ return (opcode & 0xf0) >> 4; };
		constexpr BYTE col(BYTE opcode){ return (opcode & 0x0f); };

		void log();

		constexpr void set_upper(WORD& r, BYTE value){ r &= 0xff00; r |= value; };
		constexpr void set_lower(WORD& r, BYTE value){ r &= 0x00ff; r |= (value << 8);};
		constexpr BYTE get_upper(const WORD& r){ return r & 0x00ff; };
		constexpr BYTE get_lower(const WORD& r){ return (r & 0xff00) >> 8; };

		constexpr BYTE get_B(){ return get_lower(m_registers[0]); };
		constexpr BYTE get_C(){ return get_upper(m_registers[0]); };

		constexpr BYTE get_D(){ return get_lower(m_registers[1]); };
		constexpr BYTE get_E(){ return get_upper(m_registers[1]); };

		constexpr BYTE get_H(){ return get_lower(m_registers[2]); };
		constexpr BYTE get_L(){ return get_upper(m_registers[2]); };

		constexpr BYTE get_A(){ return get_lower(m_registers[3]); };
		constexpr BYTE get_F(){ return get_upper(m_registers[3]); };

		constexpr WORD get_BC(){ return m_registers[0]; };
		constexpr WORD get_DE(){ return m_registers[1]; };
		constexpr WORD get_HL(){ return m_registers[2]; }
		constexpr WORD get_AF(){ return m_registers[3]; };

		constexpr void set_B(BYTE value){ set_lower(m_registers[0], value); };
		constexpr void set_C(BYTE value){ set_upper(m_registers[0], value); };

		constexpr void set_D(BYTE value){ set_lower(m_registers[1], value); };
		constexpr void set_E(BYTE value){ set_upper(m_registers[1], value); };

		constexpr void set_H(BYTE value){ set_lower(m_registers[2], value); };
		constexpr void set_L(BYTE value){ set_upper(m_registers[2], value); };

		constexpr void set_A(BYTE value){ set_lower(m_registers[3], value); };
		constexpr void set_F(BYTE value){ set_upper(m_registers[3], value); };

		constexpr void set_BC(WORD value) { m_registers[0] = value; };
		constexpr void set_DE(WORD value) { m_registers[1] = value; };
		constexpr void set_HL(WORD value) { m_registers[2] = value; };
		constexpr void set_AF(WORD value) { m_registers[3] = value; };

		void stack_push(BYTE value);
		BYTE stack_pop();
		void build_16bit_loads();
		void build_8bit_loads();

	private:
		enum reg16{
			BC = 0,
			DE,
			HL,
			AF
		};
		WORD m_registers[4];
		WORD m_sp;

		WORD m_pc = ENTRY_POINT;
		Mem_mu * m_memory;

		bool m_interrupts_enabled = true;
		std::function<void(void)> m_opcode_mat[16][16];
		std::map<int, std::function<BYTE(void)>> m_reg_rmap;
		std::map<int, std::function<BYTE(BYTE)>> m_reg_wmap;
		std::map<int, std::function<WORD&(void)>> m_reg16_map;
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
				  // ld r, u8    ld [XX], r
	HL_B = 0x70,  B_U8 = 0x06,  _BC_A = 0x02,	 _U16_A = 0xea,
	HL_C = 0x71,  D_U8 = 0x16,  _DE_A = 0x12,	  A_U16_ = 0xfa,
	HL_D = 0x72,  H_U8 = 0x26,  _HL_INC_A = 0x22, /************/
	HL_E = 0x73,  HL_U8 = 0x36, _HL_DEC_A = 0x32, _FF00_U8_A = 0xe0,
	HL_H = 0x74,  C_U8 = 0x0e,  A_BC_ = 0xa,      A_FF00_U8_ = 0xf0,
	HL_L = 0x75,  E_U8 = 0x1e,  A_DE_ = 0x1a,     _FF00_C_A = 0xe2,
	HL_A = 0x77,  L_U8 = 0x2e,  A_HL_INC = 0x2a,  A_FF00_C_ = 0xf2,
				  A_U8 = 0x3e,  A_HL_DEC = 0x3a,
};

enum class Gb_core::ld_16bit{
	// ld XX, u16
	BC_U16 = 0x01, POP_BC = 0xc1, PUSH_BC = 0xc5,
	DE_U16 = 0x11, POP_DE = 0xd1, PUSH_DE = 0xd5,
	HL_U16 = 0X21, POP_HL = 0xe1, PUSH_HL = 0xe5,
	SP_U16 = 0x31, POP_AF = 0xf1, PUSH_AF = 0xf5,
};

enum class Gb_core::alu{
	ADD_A_B = 0x80,  ADC_A_B = 0x88, SUB_A_B = 0x90, SBC_A_B = 0x98, AND_A_B = 0xa0,
	ADD_A_C = 0x81,  ADC_A_C = 0x89, SUB_A_C = 0x91, SBC_A_C = 0x99, AND_A_C = 0xa1,
	ADD_A_D = 0x82,  ADC_A_D = 0x8a, SUB_A_D = 0x92, SBC_A_D = 0x9a, AND_A_D = 0xa2,
	ADD_A_E = 0x83,  ADC_A_E = 0x8b, SUB_A_E = 0x93, SBC_A_E = 0x9b, AND_A_E = 0xa3,
	ADD_A_H = 0x84,  ADC_A_H = 0x8c, SUB_A_H = 0x94, SBC_A_H = 0x9c, AND_A_H = 0xa4,
	ADD_A_L = 0x85,  ADC_A_L = 0x8d, SUB_A_L = 0x95, SBC_A_L = 0x9d, AND_A_L = 0xa5,
	ADD_A_HL_ = 0x86,ADC_A_HL_ =0x8e,SUB_A_HL_ = 0x96,SBC_A_HL_ = 0x9e,AND_A_HL_ = 0xa6,
	ADD_A_A = 0x87,  ADC_A_A = 0x8f, SUB_A_A = 0x97, SBC_A_A = 0x9f, AND_A_A = 0xa7,

	XOR_A_B = 0xa8, OR_START = 0xb0,
	XOR_A_C = 0xa9, OR_END = 0xb7,
	XOR_A_D = 0xaa, CP_START = 0xb8,
	XOR_A_E = 0xab, CP_END = 0xbf,
	XOR_A_H = 0xac,
	XOR_A_L = 0xad,
	XOR_A_HL_ = 0xae,
	XOR_A_A = 0xaf,
};

enum class Gb_core::i_control{
	JMP_NN = 0xc3
};
