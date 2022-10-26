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
		enum flag{
			ZERO = 7,
			SUBS = 6,
			HALF_CARRY = 5,
			CARRY = 4
		};
		enum intrp{
			VBLANK = 0,
			LCD,
			TIMER,
			SERIAL,
			JOYPAD
		};
	public:
		Gb_core() = default;
		Gb_core(Mem_mu * memory);
		int execute_instruction();
		void init();
		void handle_interrupts();

	private:
		void init_registers();




		// alu handlers
		void x8_alu_xor(BYTE r2);
		void x8_alu_add(BYTE r2);
		void x8_alu_adc(BYTE r2);
		void x8_alu_sub(BYTE r2);
		void x8_alu_sbc(BYTE r2);
		void x8_alu_and(BYTE r2);
		void x8_alu_or(BYTE r2);
		void x8_alu_cp(BYTE r2);
		BYTE x8_alu_inc(BYTE r);
		BYTE x8_alu_dec(BYTE r);
		void x8_alu_daa(); // decimal adjust A, after arithmetic instruction
		void x8_alu_cpl(); 
		void x8_alu_ccf(); 
		void x8_alu_scf(); 

		void x16_alu_inc(WORD& rr);
		void x16_alu_dec(WORD& rr);
		void x16_alu_add(WORD rr);
		void x16_alu_addsp();

		// control handlers
		void ctrl_return(bool cond);
		void ctrl_call(bool bond);
		void ctrl_jr(bool cond);
		void ctrl_rst(const WORD offset);
		void jmp_nn(bool cond);


		void set_flag(flag f, bool set);
		constexpr BYTE get_flag(flag f){ return (get_F() >> f) & 0x1; };
		constexpr BYTE row(BYTE opcode){ return (opcode & 0xf0) >> 4; };
		constexpr BYTE col(BYTE opcode){ return (opcode & 0x0f); };

		void log();

		constexpr void set_lower(WORD& r, BYTE value){ r &= 0xff00; r |= value; };
		constexpr void set_upper(WORD& r, BYTE value){ r &= 0x00ff; r |= (value << 8);};
		constexpr BYTE get_lower(const WORD& r){ return r & 0x00ff; };
		constexpr BYTE get_upper(const WORD& r){ return (r & 0xff00) >> 8; };

		constexpr BYTE get_B(){ return get_upper(m_registers[0]); };
		constexpr BYTE get_C(){ return get_lower(m_registers[0]); };

		constexpr BYTE get_D(){ return get_upper(m_registers[1]); };
		constexpr BYTE get_E(){ return get_lower(m_registers[1]); };

		constexpr BYTE get_H(){ return get_upper(m_registers[2]); };
		constexpr BYTE get_L(){ return get_lower(m_registers[2]); };

		constexpr BYTE get_A(){ return get_upper(m_registers[3]); };
		constexpr BYTE get_F(){ return get_lower(m_registers[3]); };

		constexpr WORD get_BC(){ return m_registers[0]; };
		constexpr WORD get_DE(){ return m_registers[1]; };
		constexpr WORD get_HL(){ return m_registers[2]; }
		constexpr WORD get_AF(){ return m_registers[3]; };

		constexpr void set_B(BYTE value){ set_upper(m_registers[0], value); };
		constexpr void set_C(BYTE value){ set_lower(m_registers[0], value); };

		constexpr void set_D(BYTE value){ set_upper(m_registers[1], value); };
		constexpr void set_E(BYTE value){ set_lower(m_registers[1], value); };

		constexpr void set_H(BYTE value){ set_upper(m_registers[2], value); };
		constexpr void set_L(BYTE value){ set_lower(m_registers[2], value); };

		constexpr void set_A(BYTE value){ set_upper(m_registers[3], value); };
		constexpr void set_F(BYTE value){ set_lower(m_registers[3], value); };

		constexpr void set_BC(WORD value) { m_registers[0] = value; };
		constexpr void set_DE(WORD value) { m_registers[1] = value; };
		constexpr void set_HL(WORD value) { m_registers[2] = value; };
		constexpr void set_AF(WORD value) { m_registers[3] = value; };
		constexpr bool branch_decision(bool cond){ m_cycles++; return cond; };

		void stack_push(BYTE value);
		BYTE stack_pop();

		BYTE pc_get_byte();
		WORD pc_get_word();

		BYTE srl(BYTE r);
		BYTE rr(BYTE r);
		BYTE rrc(BYTE r);
		void rra();
		BYTE rlc(BYTE r);
		BYTE rl(BYTE r);
		BYTE swap(BYTE r); // Swap upper & lower nibles of r;
		BYTE sla(BYTE r);
		BYTE sra(BYTE r);
		void bit(BYTE r, BYTE b);
		BYTE res(BYTE r, BYTE b);
		BYTE set(BYTE r, BYTE b);

		void call_interrupt(intrp i);

		void make_mappings();
		void make_cb_mappings();

		void opcode_map(BYTE opcode, std::function<void(void)> handler){ m_opcode_mat[row(opcode)][col(opcode)] = handler;};
		void opcode_cbmap(BYTE opcode, std::function<void(void)> handler){ m_cb_mat[row(opcode)][col(opcode)] = handler; }

		void opcode_0xf8();
	private:
		WORD m_registers[4];
		WORD m_sp;
		WORD m_pc = ENTRY_POINT;

		Mem_mu * m_memory;

		bool m_interrupts_enabled = true;
		std::function<void(void)> m_opcode_mat[16][16];
		std::function<void(void)> m_cb_mat[16][16];
		std::map<intrp, WORD> m_intrp_addr;
		int m_cycles = 0;
};
