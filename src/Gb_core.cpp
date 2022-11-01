#include "Gb_core.h"
#include <variant>
#include <stdio.h>

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {};

void Gb_core::make_mappings(){
	// ld b, r
	opcode_map(0x40, [this](){ set_B(get_B()); } );
	opcode_map(0x41, [this](){ set_B(get_C()); } );
	opcode_map(0x42, [this](){ set_B(get_D()); } );
	opcode_map(0x43, [this](){ set_B(get_E()); } );
	opcode_map(0x44, [this](){ set_B(get_H()); } );
	opcode_map(0x45, [this](){ set_B(get_L()); } );
	opcode_map(0x46, [this](){ set_B(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x47, [this](){ set_B(get_A()); } );

	// ld c, r
	opcode_map(0x48, [this](){ set_C(get_B()); } );
	opcode_map(0x49, [this](){ set_C(get_C()); } );
	opcode_map(0x4a, [this](){ set_C(get_D()); } );
	opcode_map(0x4b, [this](){ set_C(get_E()); } );
	opcode_map(0x4c, [this](){ set_C(get_H()); } );
	opcode_map(0x4d, [this](){ set_C(get_L()); } );
	opcode_map(0x4e, [this](){ set_C(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x4f, [this](){ set_C(get_A()); } );

	// ld d, r
	opcode_map(0x50, [this](){ set_D(get_B()); } );
	opcode_map(0x51, [this](){ set_D(get_C()); } );
	opcode_map(0x52, [this](){ set_D(get_D()); } );
	opcode_map(0x53, [this](){ set_D(get_E()); } );
	opcode_map(0x54, [this](){ set_D(get_H()); } );
	opcode_map(0x55, [this](){ set_D(get_L()); } );
	opcode_map(0x56, [this](){ set_D(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x57, [this](){ set_D(get_A()); } );

	// ld e, r
	opcode_map(0x58, [this](){ set_E(get_B()); } );
	opcode_map(0x59, [this](){ set_E(get_C()); } );
	opcode_map(0x5a, [this](){ set_E(get_D()); } );
	opcode_map(0x5b, [this](){ set_E(get_E()); } );
	opcode_map(0x5c, [this](){ set_E(get_H()); } );
	opcode_map(0x5d, [this](){ set_E(get_L()); } );
	opcode_map(0x5e, [this](){ set_E(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x5f, [this](){ set_E(get_A()); } );

	// ld h, r
	opcode_map(0x60, [this](){ set_H(get_B()); } );
	opcode_map(0x61, [this](){ set_H(get_C()); } );
	opcode_map(0x62, [this](){ set_H(get_D()); } );
	opcode_map(0x63, [this](){ set_H(get_E()); } );
	opcode_map(0x64, [this](){ set_H(get_H()); } );
	opcode_map(0x65, [this](){ set_H(get_L()); } );
	opcode_map(0x66, [this](){ set_H(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x67, [this](){ set_H(get_A()); } );

	// ld l, r
	opcode_map(0x68, [this](){ set_L(get_B()); });
	opcode_map(0x69, [this](){ set_L(get_C()); });
	opcode_map(0x6a, [this](){ set_L(get_D()); });
	opcode_map(0x6b, [this](){ set_L(get_E()); });
	opcode_map(0x6c, [this](){ set_L(get_H()); });
	opcode_map(0x6d, [this](){ set_L(get_L()); });
	opcode_map(0x6e, [this](){ set_L(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x6f, [this](){ set_L(get_A()); });

	//ld [hl], r
	opcode_map(0x70, [this](){ m_memory->write(get_HL(), get_B()); m_cycles++; } );
	opcode_map(0x71, [this](){ m_memory->write(get_HL(), get_C()); m_cycles++; } );
	opcode_map(0x72, [this](){ m_memory->write(get_HL(), get_D()); m_cycles++; } );
	opcode_map(0x73, [this](){ m_memory->write(get_HL(), get_E()); m_cycles++; } );
	opcode_map(0x74, [this](){ m_memory->write(get_HL(), get_H()); m_cycles++; } );
	opcode_map(0x75, [this](){ m_memory->write(get_HL(), get_L()); m_cycles++; } );
	opcode_map(0x76, [this](){ halt(); });
	opcode_map(0x77, [this](){ m_memory->write(get_HL(), get_A()); m_cycles++;} );

	// ld a, r
	opcode_map(0x78, [this](){ set_A(get_B()); });
	opcode_map(0x79, [this](){ set_A(get_C()); });
	opcode_map(0x7a, [this](){ set_A(get_D()); });
	opcode_map(0x7b, [this](){ set_A(get_E()); });
	opcode_map(0x7c, [this](){ set_A(get_H()); });
	opcode_map(0x7d, [this](){ set_A(get_L()); });
	opcode_map(0x7e, [this](){ set_A(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x7f, [this](){ set_A(get_A()); });

	// ld [rr], A
	opcode_map(0x02, [this](){ m_memory->write(get_BC(), get_A()); m_cycles++; } );
	opcode_map(0x12, [this](){ m_memory->write(get_DE(), get_A()); m_cycles++; } );
	opcode_map(0x22, [this](){ m_memory->write(get_HL(), get_A()); set_HL(get_HL() + 1); m_cycles++; } );
	opcode_map(0x32, [this](){ m_memory->write(get_HL(), get_A()); set_HL(get_HL() - 1); m_cycles++; } );

	// ld r, u8
	opcode_map(0x06, [this](){ set_B(pc_get_byte()); });
	opcode_map(0x16, [this](){ set_D(pc_get_byte()); });
	opcode_map(0x26, [this](){ set_H(pc_get_byte()); });
	opcode_map(0x36, [this](){ m_memory->write(get_HL(), pc_get_byte()); m_cycles++; } );

	// ld r, u8
	opcode_map(0x0e, [this](){ set_C(pc_get_byte()); });
	opcode_map(0x1e, [this](){ set_E(pc_get_byte()); });
	opcode_map(0x2e, [this](){ set_L(pc_get_byte()); });
	opcode_map(0x3e, [this](){ set_A(pc_get_byte()); });

	opcode_map(0x0a, [this](){ set_A(m_memory->read(get_BC())); m_cycles++; });
	opcode_map(0x1a, [this](){ set_A(m_memory->read(get_DE())); m_cycles++; });
	opcode_map(0x2a, [this](){ set_A(m_memory->read(get_HL())); set_HL(get_HL() + 1); m_cycles++; });
	opcode_map(0x3a, [this](){ set_A(m_memory->read(get_HL())); set_HL(get_HL() - 1); m_cycles++; });

	opcode_map(0xe0, [this](){ m_memory->write(0xff00 + pc_get_byte(), get_A()); });
	opcode_map(0xe2, [this](){ m_memory->write(0xff00 + get_C(), get_A()); });
	opcode_map(0xf0, [this](){ set_A(m_memory->read(0xff00 + pc_get_byte())); });
	opcode_map(0xf2, [this](){ set_A(m_memory->read(0xff00 + get_C())); });

	opcode_map(0xea, [this](){ m_memory->write(pc_get_word(), get_A()); });
	opcode_map(0xfa, [this](){ set_A(m_memory->read(pc_get_word())); });


	// 16 bit loads
	opcode_map(0x01, [this](){ set_BC(pc_get_word()); });
	opcode_map(0x11, [this](){ set_DE(pc_get_word()); });
	opcode_map(0x21, [this](){ set_HL(pc_get_word()); });
	opcode_map(0x31, [this](){ m_sp = pc_get_word(); });
	opcode_map(0x08, [this](){ 
			auto addr = pc_get_word();
			m_memory->write(addr, get_lower(m_sp));
			m_memory->write(addr + 1, get_upper(m_sp));
	});

	// stack pops
	opcode_map(0xc1, [this](){ set_C(stack_pop()); set_B(stack_pop()); });
	opcode_map(0xd1, [this](){ set_E(stack_pop()); set_D(stack_pop()); });
	opcode_map(0xe1, [this](){ set_L(stack_pop()); set_H(stack_pop()); });
	opcode_map(0xf1, [this](){ set_F(stack_pop() & 0xf0); set_A(stack_pop()); });

	// stack pushes
	opcode_map(0xc5, [this](){ stack_push(get_B()); stack_push(get_C()); });
	opcode_map(0xd5, [this](){ stack_push(get_D()); stack_push(get_E()); });
	opcode_map(0xe5, [this](){ stack_push(get_H()); stack_push(get_L()); });
	opcode_map(0xf5, [this](){ stack_push(get_A()); stack_push(get_F()); });

	opcode_map(0xf9, [this](){ m_sp = get_HL(); });

	// CONTROL
	// jr
	opcode_map(0x18, [this](){ ctrl_jr(true); });
	opcode_map(0x28, [this](){ ctrl_jr(get_flag(flag::ZERO)); });
	opcode_map(0x38, [this](){ ctrl_jr(get_flag(flag::CARRY)); });
	opcode_map(0x20, [this](){ ctrl_jr(!get_flag(flag::ZERO)); });
	opcode_map(0x30, [this](){ ctrl_jr(!get_flag(flag::CARRY)); });

	// returns

	opcode_map(0x0, [this](){ return; });
	opcode_map(0xc9, [this](){ ctrl_return(true); });
	opcode_map(0xc0, [this](){ ctrl_return(!get_flag(flag::ZERO)); });
	opcode_map(0xc8, [this](){ ctrl_return(get_flag(flag::ZERO)); });
	opcode_map(0xd0, [this](){ ctrl_return(!get_flag(flag::CARRY)); });
	opcode_map(0xd8, [this](){ ctrl_return(get_flag(flag::CARRY)); });

	// calls
	
	opcode_map(0xcd, [this](){ ctrl_call(true); });
	opcode_map(0xcc, [this](){ ctrl_call(get_flag(flag::ZERO)); });
	opcode_map(0xc4, [this](){ ctrl_call(!get_flag(flag::ZERO)); });
	opcode_map(0xd4, [this](){ ctrl_call(!get_flag(flag::CARRY)); });
	opcode_map(0xdc, [this](){ ctrl_call(get_flag(flag::CARRY)); });

	// jp
	
	opcode_map(0xc3, [this](){ jmp_nn(true); });
	opcode_map(0xc2, [this](){ jmp_nn(!get_flag(flag::ZERO)); });
	opcode_map(0xca, [this](){ jmp_nn(get_flag(flag::ZERO)); });
	opcode_map(0xda, [this](){ jmp_nn(get_flag(flag::CARRY)); });
	opcode_map(0xd2, [this](){ jmp_nn(!get_flag(flag::CARRY)); });
	opcode_map(0xe9, [this](){ m_pc = get_HL(); });

	// interrupts
	opcode_map(0xf3, [this](){ m_interrupts_enabled = false; } );
	opcode_map(0xfb, [this](){ m_interrupts_enabled = true; } );
	opcode_map(0xd9, [this](){ m_interrupts_enabled = true; ctrl_return(true); m_cycles--; } );

	// rst
	opcode_map(0xc7, [this]() { ctrl_rst(0x00); } );
	opcode_map(0xd7, [this]() { ctrl_rst(0x10); } );
	opcode_map(0xe7, [this]() { ctrl_rst(0x20); } );
	opcode_map(0xf7, [this]() { ctrl_rst(0x30); } );
	opcode_map(0xcf, [this]() { ctrl_rst(0x08); } );
	opcode_map(0xdf, [this]() { ctrl_rst(0x18); } );
	opcode_map(0xef, [this]() { ctrl_rst(0x28); } );
	opcode_map(0xff, [this]() { ctrl_rst(0x38); } );

	// x16_alu
	opcode_map(0x03, [this]() { set_BC(get_BC() + 1); m_cycles++; });
	opcode_map(0x13, [this]() { set_DE(get_DE() + 1); m_cycles++; });
	opcode_map(0x23, [this]() { set_HL(get_HL() + 1); m_cycles++; });
	opcode_map(0x33, [this]() { m_sp++; m_cycles += 2; });

	opcode_map(0x0b, [this]() { set_BC(get_BC() - 1); m_cycles++; });
	opcode_map(0x1b, [this]() { set_DE(get_DE() - 1); m_cycles++; });
	opcode_map(0x2b, [this]() { set_HL(get_HL() - 1); m_cycles++; });
	opcode_map(0x3b, [this]() { m_sp--; m_cycles++; });

	opcode_map(0x09, [this]() { x16_alu_add(get_BC()); });
	opcode_map(0x19, [this]() { x16_alu_add(get_DE()); });
	opcode_map(0x29, [this]() { x16_alu_add(get_HL()); });
	opcode_map(0x39, [this]() { x16_alu_add(m_sp); });

	opcode_map(0xe8, [this]() { x16_alu_addsp(); });
	opcode_map(0xf8, [this]() { opcode_0xf8(); });

	// x8/alu
	// add r
	opcode_map(0x80, [this](){ x8_alu_add(get_B()); });
	opcode_map(0x81, [this](){ x8_alu_add(get_C()); });
	opcode_map(0x82, [this](){ x8_alu_add(get_D()); });
	opcode_map(0x83, [this](){ x8_alu_add(get_E()); });
	opcode_map(0x84, [this](){ x8_alu_add(get_H()); });
	opcode_map(0x85, [this](){ x8_alu_add(get_L()); });
	opcode_map(0x86, [this](){ x8_alu_add(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x87, [this](){ x8_alu_add(get_A()); });

	// adc r
	opcode_map(0x88, [this](){ x8_alu_adc(get_B()); });
	opcode_map(0x89, [this](){ x8_alu_adc(get_C()); });
	opcode_map(0x8a, [this](){ x8_alu_adc(get_D()); });
	opcode_map(0x8b, [this](){ x8_alu_adc(get_E()); });
	opcode_map(0x8c, [this](){ x8_alu_adc(get_H()); });
	opcode_map(0x8d, [this](){ x8_alu_adc(get_L()); });
	opcode_map(0x8e, [this](){ x8_alu_adc(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x8f, [this](){ x8_alu_adc(get_A()); });

	// sub r
	opcode_map(0x90, [this](){ x8_alu_sub(get_B()); });
	opcode_map(0x91, [this](){ x8_alu_sub(get_C()); });
	opcode_map(0x92, [this](){ x8_alu_sub(get_D()); });
	opcode_map(0x93, [this](){ x8_alu_sub(get_E()); });
	opcode_map(0x94, [this](){ x8_alu_sub(get_H()); });
	opcode_map(0x95, [this](){ x8_alu_sub(get_L()); });
	opcode_map(0x96, [this](){ x8_alu_sub(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x97, [this](){ x8_alu_sub(get_A()); });

	// sbc r
	opcode_map(0x98, [this](){ x8_alu_sbc(get_B()); });
	opcode_map(0x99, [this](){ x8_alu_sbc(get_C()); });
	opcode_map(0x9a, [this](){ x8_alu_sbc(get_D()); });
	opcode_map(0x9b, [this](){ x8_alu_sbc(get_E()); });
	opcode_map(0x9c, [this](){ x8_alu_sbc(get_H()); });
	opcode_map(0x9d, [this](){ x8_alu_sbc(get_L()); });
	opcode_map(0x9e, [this](){ x8_alu_sbc(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0x9f, [this](){ x8_alu_sbc(get_A()); });

	// and r
	opcode_map(0xa0, [this](){ x8_alu_and(get_B()); });
	opcode_map(0xa1, [this](){ x8_alu_and(get_C()); });
	opcode_map(0xa2, [this](){ x8_alu_and(get_D()); });
	opcode_map(0xa3, [this](){ x8_alu_and(get_E()); });
	opcode_map(0xa4, [this](){ x8_alu_and(get_H()); });
	opcode_map(0xa5, [this](){ x8_alu_and(get_L()); });
	opcode_map(0xa6, [this](){ x8_alu_and(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0xa7, [this](){ x8_alu_and(get_A()); });

	// xor r
	opcode_map(0xa8, [this](){ x8_alu_xor(get_B()); });
	opcode_map(0xa9, [this](){ x8_alu_xor(get_C()); });
	opcode_map(0xaa, [this](){ x8_alu_xor(get_D()); });
	opcode_map(0xab, [this](){ x8_alu_xor(get_E()); });
	opcode_map(0xac, [this](){ x8_alu_xor(get_H()); });
	opcode_map(0xad, [this](){ x8_alu_xor(get_L()); });
	opcode_map(0xae, [this](){ x8_alu_xor(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0xaf, [this](){ x8_alu_xor(get_A()); });

	// or r
	opcode_map(0xb0, [this](){ x8_alu_or(get_B()); });
	opcode_map(0xb1, [this](){ x8_alu_or(get_C()); });
	opcode_map(0xb2, [this](){ x8_alu_or(get_D()); });
	opcode_map(0xb3, [this](){ x8_alu_or(get_E()); });
	opcode_map(0xb4, [this](){ x8_alu_or(get_H()); });
	opcode_map(0xb5, [this](){ x8_alu_or(get_L()); });
	opcode_map(0xb6, [this](){ x8_alu_or(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0xb7, [this](){ x8_alu_or(get_A()); });

	// cp r
	opcode_map(0xb8, [this](){ x8_alu_cp(get_B()); });
	opcode_map(0xb9, [this](){ x8_alu_cp(get_C()); });
	opcode_map(0xba, [this](){ x8_alu_cp(get_D()); });
	opcode_map(0xbb, [this](){ x8_alu_cp(get_E()); });
	opcode_map(0xbc, [this](){ x8_alu_cp(get_H()); });
	opcode_map(0xbd, [this](){ x8_alu_cp(get_L()); });
	opcode_map(0xbe, [this](){ x8_alu_cp(m_memory->read(get_HL())); m_cycles++; });
	opcode_map(0xbf, [this](){ x8_alu_cp(get_A()); });

	// inc
	opcode_map(0x04, [this](){ set_B(x8_alu_inc(get_B())); });
	opcode_map(0x14, [this](){ set_D(x8_alu_inc(get_D())); });
	opcode_map(0x24, [this](){ set_H(x8_alu_inc(get_H())); });
	opcode_map(0x34, [this](){ 
		BYTE value = m_memory->read(get_HL());
		m_memory->write(get_HL(), x8_alu_inc(value));
		m_cycles += 2;
	});

	opcode_map(0x0c, [this](){ set_C(x8_alu_inc(get_C())); });
	opcode_map(0x1c, [this](){ set_E(x8_alu_inc(get_E())); });
	opcode_map(0x2c, [this](){ set_L(x8_alu_inc(get_L())); });
	opcode_map(0x3c, [this](){ set_A(x8_alu_inc(get_A())); });

	// dec
	opcode_map(0x05, [this](){ set_B(x8_alu_dec(get_B())); });
	opcode_map(0x15, [this](){ set_D(x8_alu_dec(get_D())); });
	opcode_map(0x25, [this](){ set_H(x8_alu_dec(get_H())); });
	opcode_map(0x35, [this](){ 
		BYTE value = m_memory->read(get_HL());
		m_memory->write(get_HL(), x8_alu_dec(value));
		m_cycles += 2;
	});

	opcode_map(0x0d, [this](){ set_C(x8_alu_dec(get_C())); });
	opcode_map(0x1d, [this](){ set_E(x8_alu_dec(get_E())); });
	opcode_map(0x2d, [this](){ set_L(x8_alu_dec(get_L())); });
	opcode_map(0x3d, [this](){ set_A(x8_alu_dec(get_A())); });

	// misc alu

	opcode_map(0xc6, [this](){ x8_alu_add(pc_get_byte()); });
	opcode_map(0xd6, [this](){ x8_alu_sub(pc_get_byte()); });
	opcode_map(0xe6, [this](){ x8_alu_and(pc_get_byte()); });
	opcode_map(0xf6, [this](){ x8_alu_or(pc_get_byte()); });

	opcode_map(0xce, [this](){ x8_alu_adc(pc_get_byte()); });
	opcode_map(0xde, [this](){ x8_alu_sbc(pc_get_byte()); });
	opcode_map(0xee, [this](){ x8_alu_xor(pc_get_byte()); });
	opcode_map(0xfe, [this](){ x8_alu_cp(pc_get_byte()); });

	opcode_map(0x27, [this](){ x8_alu_daa(); });
	opcode_map(0x37, [this](){ x8_alu_scf(); });
	opcode_map(0x2f, [this](){ x8_alu_cpl(); });
	opcode_map(0x3f, [this](){ x8_alu_ccf(); });

	// x8 rsb

	opcode_map(0xcb, [this](){ // forward opcode to m_cb_mat
		auto opcode = pc_get_byte();
		auto op_handler = m_cb_mat[ROW(opcode)][COL(opcode)];
		if(op_handler != nullptr){
			op_handler();
		}else{
			std::cerr << "Unknown cb opcode : " << std::hex << (int)opcode << std::endl;
			exit(1);
		}
	});

	// misc
	opcode_map(0x1f, [this](){ set_A(rr(get_A())); set_flag(flag::ZERO,false); });
	opcode_map(0x07, [this](){ set_A(rlc(get_A())); set_flag(flag::ZERO, false); });
	opcode_map(0x17, [this](){ set_A(rl(get_A())); set_flag(flag::ZERO, false); });
	opcode_map(0x0f, [this](){ set_A(rrc(get_A())); set_flag(flag::ZERO, false); });
	opcode_map(0x10, [this](){ halt(); });
}

void Gb_core::make_cb_mappings(){
	// srl
	opcode_cbmap(0x38, [this](){ set_B(srl(get_B())); });
	opcode_cbmap(0x39, [this](){ set_C(srl(get_C())); });
	opcode_cbmap(0x3a, [this](){ set_D(srl(get_D())); });
	opcode_cbmap(0x3b, [this](){ set_E(srl(get_E())); });
	opcode_cbmap(0x3c, [this](){ set_H(srl(get_H())); });
	opcode_cbmap(0x3d, [this](){ set_L(srl(get_L())); });
	opcode_cbmap(0x3e, [this](){ 
		auto res = srl(m_memory->read(get_HL()));
		m_memory->write(get_HL(), res);
	});
	opcode_cbmap(0x3f, [this](){ set_A(srl(get_A())); });

	//rr : rotate right trough carry flag
	opcode_cbmap(0x18, [this](){ set_B(rr(get_B())); });
	opcode_cbmap(0x19, [this](){ set_C(rr(get_C())); });
	opcode_cbmap(0x1a, [this](){ set_D(rr(get_D())); });
	opcode_cbmap(0x1b, [this](){ set_E(rr(get_E())); });
	opcode_cbmap(0x1c, [this](){ set_H(rr(get_H())); });
	opcode_cbmap(0x1d, [this](){ set_L(rr(get_L())); });
	opcode_cbmap(0x1e, [this](){
		auto res = rr(m_memory->read(get_HL()));
		m_memory->write(get_HL(), res);
	});
	opcode_cbmap(0x1f, [this](){ set_A(rr(get_A())); });

	// swap upper and lower nibles
	opcode_cbmap(0x30, [this](){ set_B(swap(get_B())); });
	opcode_cbmap(0x31, [this](){ set_C(swap(get_C())); });
	opcode_cbmap(0x32, [this](){ set_D(swap(get_D())); });
	opcode_cbmap(0x33, [this](){ set_E(swap(get_E())); });
	opcode_cbmap(0x34, [this](){ set_H(swap(get_H())); });
	opcode_cbmap(0x35, [this](){ set_L(swap(get_L())); });
	opcode_cbmap(0x36, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), swap(value));
		m_cycles += 2;
	});
	opcode_cbmap(0x37, [this](){ set_A(swap(get_A())); });

	// rlc : rotate left to carry flag.
	opcode_cbmap(0x00, [this](){ set_B(rlc(get_B())); });
	opcode_cbmap(0x01, [this](){ set_C(rlc(get_C())); });
	opcode_cbmap(0x02, [this](){ set_D(rlc(get_D())); });
	opcode_cbmap(0x03, [this](){ set_E(rlc(get_E())); });
	opcode_cbmap(0x04, [this](){ set_H(rlc(get_H())); });
	opcode_cbmap(0x05, [this](){ set_L(rlc(get_L())); });
	opcode_cbmap(0x06, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), rlc(value));
		m_cycles += 2;
	});

	opcode_cbmap(0x07, [this](){ set_A(rlc(get_A())); });

	// rrc : rotate right to carry flag
	opcode_cbmap(0x08, [this](){ set_B(rrc(get_B())); });
	opcode_cbmap(0x09, [this](){ set_C(rrc(get_C())); });
	opcode_cbmap(0x0a, [this](){ set_D(rrc(get_D())); });
	opcode_cbmap(0x0b, [this](){ set_E(rrc(get_E())); });
	opcode_cbmap(0x0c, [this](){ set_H(rrc(get_H())); });
	opcode_cbmap(0x0d, [this](){ set_L(rrc(get_L())); });
	opcode_cbmap(0x0e, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), rrc(value));
		m_cycles += 2;
	});
	opcode_cbmap(0x0f, [this](){ set_A(rrc(get_A())); });

	// rlc : rotate left through carry flag.
	opcode_cbmap(0x10, [this](){ set_B(rl(get_B())); });
	opcode_cbmap(0x11, [this](){ set_C(rl(get_C())); });
	opcode_cbmap(0x12, [this](){ set_D(rl(get_D())); });
	opcode_cbmap(0x13, [this](){ set_E(rl(get_E())); });
	opcode_cbmap(0x14, [this](){ set_H(rl(get_H())); });
	opcode_cbmap(0x15, [this](){ set_L(rl(get_L())); });
	opcode_cbmap(0x16, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), rl(value));
		m_cycles += 2;
	});

	opcode_cbmap(0x17, [this](){ set_A(rl(get_A())); });

	// sla : shift left into carry
	opcode_cbmap(0x20, [this](){ set_B(sla(get_B())); });
	opcode_cbmap(0x21, [this](){ set_C(sla(get_C())); });
	opcode_cbmap(0x22, [this](){ set_D(sla(get_D())); });
	opcode_cbmap(0x23, [this](){ set_E(sla(get_E())); });
	opcode_cbmap(0x24, [this](){ set_H(sla(get_H())); });
	opcode_cbmap(0x25, [this](){ set_L(sla(get_L())); });
	opcode_cbmap(0x26, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), sla(value));
		m_cycles += 2;
	});

	opcode_cbmap(0x27, [this](){ set_A(sla(get_A())); });

	// sra
	opcode_cbmap(0x28, [this](){ set_B(sra(get_B())); });
	opcode_cbmap(0x29, [this](){ set_C(sra(get_C())); });
	opcode_cbmap(0x2a, [this](){ set_D(sra(get_D())); });
	opcode_cbmap(0x2b, [this](){ set_E(sra(get_E())); });
	opcode_cbmap(0x2c, [this](){ set_H(sra(get_H())); });
	opcode_cbmap(0x2d, [this](){ set_L(sra(get_L())); });
	opcode_cbmap(0x2e, [this](){
		WORD value = m_memory->read(get_HL());
		m_memory->write(get_HL(), sra(value));
		m_cycles += 2;
	});
	opcode_cbmap(0x2f, [this](){ set_A(sra(get_A())); });


	// bit : test if bit b is set in register r.
	BYTE b = 0;
	for(int opcode = 0x40; opcode <= 0x7f - 7; opcode += 8){
		opcode_cbmap(opcode, [this, b](){ bit(get_B(), b); });
		opcode_cbmap(opcode + 1, [this, b](){ bit(get_C(), b); });
		opcode_cbmap(opcode + 2, [this, b](){ bit(get_D(), b); });
		opcode_cbmap(opcode + 3, [this, b](){ bit(get_E(), b); });
		opcode_cbmap(opcode + 4, [this, b](){ bit(get_H(), b); });
		opcode_cbmap(opcode + 5, [this, b](){ bit(get_L(), b); });
		opcode_cbmap(opcode + 6, [this, b](){ bit(m_memory->read(get_HL()), b); m_cycles++; });
		opcode_cbmap(opcode + 7, [this, b](){ bit(get_A(), b); });
		b++;
	}

	// res: reset bit b in register r.
	b = 0;
	for(int opcode = 0x80; opcode <= 0xbf - 7; opcode += 8){
		opcode_cbmap(opcode, [this, b](){ set_B(res(get_B(), b)); });
		opcode_cbmap(opcode + 1, [this, b](){ set_C(res(get_C(), b)); });
		opcode_cbmap(opcode + 2, [this, b](){ set_D(res(get_D(), b)); });
		opcode_cbmap(opcode + 3, [this, b](){ set_E(res(get_E(), b)); });
		opcode_cbmap(opcode + 4, [this, b](){ set_H(res(get_H(), b)); });
		opcode_cbmap(opcode + 5, [this, b](){ set_L(res(get_L(), b)); });
		opcode_cbmap(opcode + 6, [this, b](){
			BYTE value = res(m_memory->read(get_HL()), b);
			m_memory->write(get_HL(), value);
			m_cycles += 2;
		});
		opcode_cbmap(opcode + 7, [this, b](){ set_A(res(get_A(), b)); });
		b++;
	}
	// set: set bit b in register r
	b = 0;
	for(int opcode = 0xc0; opcode <= 0xff - 7; opcode += 8){
		opcode_cbmap(opcode, [this, b](){ set_B(set(get_B(), b)); });
		opcode_cbmap(opcode + 1, [this, b](){ set_C(set(get_C(), b)); });
		opcode_cbmap(opcode + 2, [this, b](){ set_D(set(get_D(), b)); });
		opcode_cbmap(opcode + 3, [this, b](){ set_E(set(get_E(), b)); });
		opcode_cbmap(opcode + 4, [this, b](){ set_H(set(get_H(), b)); });
		opcode_cbmap(opcode + 5, [this, b](){ set_L(set(get_L(), b)); });
		opcode_cbmap(opcode + 6, [this, b](){
			BYTE value = set(m_memory->read(get_HL()), b);
			m_memory->write(get_HL(), value);
			m_cycles += 2;
		});
		opcode_cbmap(opcode + 7, [this, b](){ set_A(set(get_A(), b)); });
		b++;
	}
};

void Gb_core::init_registers(){
	memset(m_registers, 0, sizeof(WORD) * 4);
	set_BC(0x0013);
	set_DE(0x00d8);
	set_HL(0x014d);
    set_AF(0x01B0);

	m_sp = SP_INIT_ADDR;
};

void Gb_core::init(){
	init_registers();
	make_mappings();
	make_cb_mappings();

	m_intrp_addr[intrp::VBLANK] = 0x40;
	m_intrp_addr[intrp::LCD] = 0x48;
	m_intrp_addr[intrp::TIMER] = 0x50;
	m_intrp_addr[intrp::SERIAL] = 0x58;
	m_intrp_addr[intrp::JOYPAD] = 0x60;
};

// real cycles not taken into account for now.
int Gb_core::execute_instruction(){
	if(m_halted) return 4;
	//log();
	BYTE opcode = pc_get_byte();
	m_cycles = 0;
	auto opcode_handler = m_opcode_mat[ROW(opcode)][COL(opcode)];
	if(opcode_handler != nullptr){
		opcode_handler();
	}else{
		std::cerr << "Unknown opcode : " << std::hex << (int)opcode << std::endl;
		exit(1);
	}
	/*
	if(m_haltbug){
		m_pc--;
		m_haltbug = false;
	}
	*/
	return m_cycles * 4;
};

void Gb_core::jmp_nn(bool cond){ 
	if(branch_decision(cond)){
		m_pc = pc_get_word();
		return;
	};
	m_pc += 2;
	m_cycles++; // warning
};

BYTE Gb_core::stack_pop(){
	m_cycles++;
	BYTE r = m_memory->read(m_sp++);
	return r;
};

void Gb_core::stack_push(BYTE value){ m_memory->write(--m_sp, value);  m_cycles++; };

void Gb_core::set_flag(Gb_core::flag f, bool set){
	set_F(set ? (get_F() | (0x1 << f)) : (get_F() & ~(0x1 << f)));
};

void Gb_core::x8_alu_add(BYTE r2){
	BYTE r1 = get_A();

	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, (((r1 & 0x0f) + (r2 & 0x0f)) & 0x10));
	set_flag(flag::CARRY, (static_cast<WORD>(r1) + static_cast<WORD>(r2) & 0x100));
	set_flag(flag::ZERO, static_cast<BYTE>(r1 + r2) == 0);

	set_A(r1 + r2);
};

void Gb_core::x8_alu_adc(BYTE r2){
	BYTE r1 = get_A();
	BYTE c_flag = get_flag(flag::CARRY);

	set_flag(flag::ZERO, ((r1 + r2 + c_flag) & 0xff) == 0);
	set_flag(flag::HALF_CARRY, (((r1 & 0x0f) + (r2 & 0x0f) + c_flag) & 0x10));
	set_flag(flag::CARRY, (static_cast<WORD>(r1) + static_cast<WORD>(r2) + c_flag > 0xff));
	set_flag(flag::SUBS, false);

	set_A(r1 + r2 + c_flag);
};

void Gb_core::x8_alu_sub(BYTE r2){
	BYTE r1 = get_A();

	set_flag(flag::SUBS, true);
	set_flag(flag::ZERO, r1 - r2 == 0);
	set_flag(flag::HALF_CARRY, (static_cast<int>(r1 & 0x0f) < static_cast<int>(r2 & 0x0f)));
	set_flag(flag::CARRY, r1 < r2);

	set_A(r1 - r2);
};

void Gb_core::x8_alu_sbc(BYTE r2){
	BYTE r1 = get_A();
	set_flag(flag::SUBS, true);

	BYTE c_flag = get_flag(flag::CARRY);
	set_flag(flag::ZERO, ((r1 - (r2 + c_flag)) & 0xff) == 0);

	set_flag(flag::HALF_CARRY, (static_cast<int>(r1 & 0x0f) < (static_cast<int>(r2 & 0x0f) + c_flag)));
	set_flag(flag::CARRY, r1 < (r2 + c_flag));

	set_A(r1 - (r2 + c_flag));
};

void Gb_core::x8_alu_and(BYTE r2){
	auto res = get_A() & r2;
	
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, true);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
};

void Gb_core::x8_alu_xor(BYTE r2){
	auto res = get_A() ^ r2;
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
};

void Gb_core::x8_alu_or(BYTE r2){
	auto res = get_A() | r2;

	set_flag(flag::ZERO, res  == 0);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
};


void Gb_core::x8_alu_cp(BYTE r2){
	BYTE a = get_A();

	set_flag(flag::SUBS, true);
	set_flag(flag::ZERO, a == r2);
	set_flag(flag::HALF_CARRY, ((a & 0x0f) - (r2 & 0x0f) < 0));
	set_flag(flag::CARRY, a < r2);
	m_cycles++;
};

BYTE Gb_core::x8_alu_inc(BYTE r){
	BYTE res = r + 1;

	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x00));

	return res;
};

BYTE Gb_core::x8_alu_dec(BYTE r){
	BYTE res = r - 1;
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, true);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x0f));
	return res;
};

void Gb_core::x8_alu_daa(){
	auto A = get_A();
	BYTE res, correction = 0;

	if(get_flag(flag::HALF_CARRY) || (!get_flag(flag::SUBS) && (A & 0xf) > 9)){
		correction |= 0x06;
	};

	if(get_flag(flag::CARRY) || (!get_flag(flag::SUBS) && (A > 0x99))){
		correction |= 0x60;
		set_flag(flag::CARRY, true);
	};

	res = get_flag(flag::SUBS) ? A - correction : A + correction;
	
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, false);
	set_A(res);
	m_cycles++;
};

void Gb_core::x8_alu_cpl(){
	set_A(~get_A());
	set_flag(flag::SUBS, true);
	set_flag(flag::HALF_CARRY, true);
};

void Gb_core::x8_alu_ccf(){
	set_flag(flag::CARRY, !get_flag(flag::CARRY));
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::SUBS, false);
};

void Gb_core::x8_alu_scf(){
	set_flag(flag::CARRY, true);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::SUBS, false);
};

void Gb_core::ctrl_return(bool cond){
	if(branch_decision(cond)){
		set_lower(m_pc, stack_pop());	
		set_upper(m_pc, stack_pop());	
		return;
	}
};

void Gb_core::ctrl_call(bool cond){
	if(branch_decision(cond)){
		BYTE addr_lower = pc_get_byte();
		BYTE addr_upper = pc_get_byte();

		stack_push(get_upper(m_pc));
		stack_push(get_lower(m_pc));

		auto routine_addr = addr_lower | (addr_upper << 8);
		m_pc = routine_addr;

	}else{
		m_pc += 2;
		m_cycles++; // warning, 
	}
};

void Gb_core::ctrl_jr(bool cond){
	if(branch_decision(cond)){
		auto offset = static_cast<signed char>(pc_get_byte());
		m_pc += offset;
	}else{
		m_pc += 1;
	}
};

void Gb_core::ctrl_rst(const WORD offset){
	stack_push(get_upper(m_pc));
	stack_push(get_lower(m_pc));
	m_pc = offset;
	m_cycles++;
};

void Gb_core::x16_alu_add(WORD rr){
	set_flag(flag::SUBS, false);
	auto hl = get_HL();

	set_flag(flag::CARRY, ((static_cast<unsigned int>(hl) + static_cast<unsigned int>(rr)) > 0xffff));
	set_flag(flag::HALF_CARRY, (((static_cast<WORD>(hl) & 0xfff) + (static_cast<WORD>(rr) & 0xfff)) & 0x1000));

	set_HL(hl + rr);
	m_cycles++;
};

void Gb_core::x16_alu_addsp(){
	SIGNED_BYTE value = static_cast<SIGNED_BYTE>(pc_get_byte());

	set_flag(flag::ZERO, false);
	set_flag(flag::SUBS, false);

	set_flag(flag::HALF_CARRY, (((m_sp & 0xf) + (value & 0xf)) & 0x10));
	set_flag(flag::CARRY, (((m_sp & 0xff) + (value & 0xff)) & 0x100));

	m_sp += value; // modifying 16-bit registers takes 4t?

	m_cycles += 2;
};

void Gb_core::opcode_0xf8(){
	SIGNED_BYTE value = static_cast<SIGNED_BYTE>(pc_get_byte());

	set_flag(flag::ZERO, false);
	set_flag(flag::SUBS, false);

	set_flag(flag::HALF_CARRY, (((m_sp & 0xf) + (value & 0xf)) & 0x10));
	set_flag(flag::CARRY, (((m_sp & 0xff) + (value & 0xff)) & 0x100));

	set_HL(m_sp + value);
	m_cycles++;
};

WORD Gb_core::pc_get_word(){ 
	WORD value = pc_get_byte() | (pc_get_byte() << 8);
	return value;
};

BYTE Gb_core::srl(BYTE r){
	set_flag(flag::CARRY, r & 0x1);
	r = r >> 1;
	set_flag(flag::ZERO, r == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	return r;
};

BYTE Gb_core::rr(BYTE r){
	auto _0bit = r & 0x1;

	r >>= 1;
	r |= (get_flag(flag::CARRY) << 7);

	set_flag(flag::CARRY, _0bit);
	set_flag(flag::ZERO, r == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);

	return r;
};

void Gb_core::call_interrupt(intrp i){
	// two waits are executed first
	m_cycles += 2;
	// prevent further interrupts
	m_interrupts_enabled = false;

	// acknoledge interrupt
	auto intrp_flag = m_memory->read(IF_ADDR);
	intrp_flag &= ~(0x1 << i);
	m_memory->write(IF_ADDR, intrp_flag);

	stack_push(get_upper(m_pc));
	stack_push(get_lower(m_pc));

	// call interrupt handler
	assert(m_intrp_addr.find(i) != m_intrp_addr.end());
	m_pc = m_intrp_addr[i];
};

void Gb_core::handle_interrupts(){
	if(interrups_pending()){
		m_halted = false;
	};

	auto intrp_flag = m_memory->read(IF_ADDR);
	auto intrp_enbaled = m_memory->read(IE_ADDR);

	if(!m_interrupts_enabled) return; // di
	if(intrp_flag == 0) return; // no requests

	for(int i = 0; i < 5; i++){
		if(((intrp_flag >> i) & 0x1) && ((intrp_enbaled >> i) & 0x1)){
			call_interrupt(static_cast<intrp>(i));
		}
	};
};

BYTE Gb_core::swap(BYTE r){
	BYTE lo_nible = r & 0x0f;
	BYTE hi_nible = (r & 0xf0) >> 4;

	BYTE res = (lo_nible << 4) | hi_nible;

	set_flag(flag::ZERO, res == 0);
	set_flag(flag::CARRY, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::SUBS, false);

	return res;
};

BYTE Gb_core::rlc(BYTE r){
	BYTE _7bit = (r >> 7) & 0x1;
	r = r << 1 | _7bit;

	set_flag(flag::CARRY, _7bit);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::ZERO, r == 0);

	return r;
};

BYTE Gb_core::rl(BYTE r){
	BYTE _7bit = (r >> 7) & 0x1;
	r = r << 1 | get_flag(flag::CARRY);

	set_flag(flag::CARRY, _7bit);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::ZERO, r == 0);

	return r;
};

BYTE Gb_core::rrc(BYTE r){
	BYTE _0bit = r & 0x1;
	r = r >> 1 | (r << 7);

	set_flag(flag::CARRY, _0bit);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::ZERO, r == 0);

	return r;
};

BYTE Gb_core::sla(BYTE r){
	BYTE _7bit = (r >> 7) & 0x1;
	r <<= 1;

	set_flag(flag::CARRY, _7bit);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::ZERO, r == 0);

	return r;
};

BYTE Gb_core::sra(BYTE r){
	set_flag(flag::CARRY, r & 0x1);

	BYTE _7bit = r & 0x80;
	r = r >> 1 | _7bit;

	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::ZERO, r == 0);

	return r;
};


void Gb_core::bit(BYTE r, BYTE b){
	set_flag(flag::ZERO, !( r & (0x1 << b)));
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, true);
};

BYTE Gb_core::res(BYTE r, BYTE b){
	r &= ~(0x1 << b);
	return r;
};

BYTE Gb_core::set(BYTE r, BYTE b){
	r |= (1 << b);
	return r;
};

void Gb_core::halt(){
	if(m_interrupts_enabled){
		m_halted = true;
		return;
	}else{
		if(interrups_pending()){
			m_halted = false;
			m_haltbug = true;
			return;
		}else{
			m_halted = true;
		}
	}
};

bool Gb_core::interrups_pending() const{
	BYTE intrp_flags = m_memory->read(Mem_mu::io_port::IF);
	BYTE intrp_enabled = m_memory->read(Mem_mu::io_port::IE);
	return intrp_flags & intrp_enabled & 0x1f;
};

void Gb_core::log(){
	printf("A: %02hhX ", get_A());
	printf("F: %02hhX ", get_F());
	printf("B: %02hhX ", get_B());
	printf("C: %02hhX ", get_C());
	printf("D: %02hhX ", get_D());
	printf("E: %02hhX ", get_E());
	printf("H: %02hhX ", get_H());
	printf("L: %02hhX ", get_L());
	printf("SP: %04X ", (unsigned int)m_sp);
	printf("PC: 00:%04X ", (unsigned int)m_pc);

	BYTE tmp;
	auto opcode = m_memory->read(m_pc);
	tmp = opcode;
	printf("(%02hhX ", (unsigned int)opcode);
	opcode = m_memory->read(m_pc + 1);
	printf("%02hhX ", (unsigned int)opcode);
	opcode = m_memory->read(m_pc + 2);
	printf("%02hhX ", (unsigned int)opcode);
	opcode = m_memory->read(m_pc + 3);
	printf("%02hhX)\n", (unsigned int)opcode);
	//printf("op: %02hhX )\n", (unsigned int)tmp);
};

BYTE Gb_core::pc_get_byte(){ 
	auto byte = m_memory->read(m_pc++); 
	m_cycles++;
	return byte;
};
