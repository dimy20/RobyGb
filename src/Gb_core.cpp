#include "Gb_core.h"
#include <variant>
#include <stdio.h>

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {};

void Gb_core::build_control(){
	// returns
	m_opcode_mat[0x0][0x0] = [this](){ return; };
	m_opcode_mat[0xc][0x9] = [this](){ ctrl_return(); };
	m_opcode_mat[0xc][0x0] = [this](){ if(!get_flag(flag::ZERO)) ctrl_return(); };
	m_opcode_mat[0xc][0x8] = [this](){ if(get_flag(flag::ZERO)) ctrl_return(); };
	m_opcode_mat[0xd][0x0] = [this](){ if(!get_flag(flag::CARRY)) ctrl_return(); };
	m_opcode_mat[0xd][0x4] = [this](){ if(get_flag(flag::CARRY)) ctrl_return(); };
	// calls
	m_opcode_mat[0xc][0xd] = [this](){ ctrl_call(true); };
	m_opcode_mat[0xc][0xc] = [this](){ ctrl_call(get_flag(flag::ZERO)); };
	m_opcode_mat[0xc][0x4] = [this](){ ctrl_call(!get_flag(flag::ZERO)); };
	m_opcode_mat[0xd][0x4] = [this](){ ctrl_call(!get_flag(flag::CARRY)); };
	m_opcode_mat[0xd][0xc] = [this](){ ctrl_call(get_flag(flag::CARRY)); };
	// jumps
	m_opcode_mat[0xc][0x3] = [this](){ jmp_nn(); };
	m_opcode_mat[0xc][0x2] = [this](){ if(!get_flag(flag::ZERO)) jmp_nn(); };
	m_opcode_mat[0xc][0xa] = [this](){ if(get_flag(flag::ZERO)) jmp_nn(); };
	m_opcode_mat[0xd][0xa] = [this](){ if(get_flag(flag::CARRY)) jmp_nn(); };
	m_opcode_mat[0xd][0x2] = [this](){ if(!get_flag(flag::CARRY)) jmp_nn(); };
	m_opcode_mat[0xe][0x9] = [this](){ m_pc = m_memory->read(get_HL()); };
	// jp offset
	m_opcode_mat[0x1][0x8] = [this](){ ctrl_jr(true); };
	m_opcode_mat[0x2][0x8] = [this](){ ctrl_jr(get_flag(flag::ZERO)); };
	m_opcode_mat[0x3][0x8] = [this](){ ctrl_jr(get_flag(flag::CARRY)); };
	m_opcode_mat[0x2][0x0] = [this](){ ctrl_jr(!get_flag(flag::ZERO)); };
	m_opcode_mat[0x3][0x0] = [this](){ ctrl_jr(!get_flag(flag::CARRY)); };
	// ime
	m_opcode_mat[0xf][0x3] = [this](){ m_interrupts_enabled = false; };
	m_opcode_mat[0xf][0xb] = [this](){ m_interrupts_enabled = true; };
	m_opcode_mat[0xd][0x9] = [this](){ m_interrupts_enabled = true; ctrl_return(); };
	//rst
	m_opcode_mat[0xc][0x7] = [this]() { ctrl_rst(0x00); };
	m_opcode_mat[0xd][0x7] = [this]() { ctrl_rst(0x10); };
	m_opcode_mat[0xe][0x7] = [this]() { ctrl_rst(0x20); };
	m_opcode_mat[0xf][0x7] = [this]() { ctrl_rst(0x30); };
	m_opcode_mat[0xc][0xf] = [this]() { ctrl_rst(0x08); };
	m_opcode_mat[0xd][0xf] = [this]() { ctrl_rst(0x18); };
	m_opcode_mat[0xe][0xf] = [this]() { ctrl_rst(0x28); };
	m_opcode_mat[0xf][0xf] = [this]() { ctrl_rst(0x38); };

	// x16 alu arithmetic mappings
	m_opcode_mat[0x0][0x3] = [this]() { set_BC(get_BC() + 1); };
	m_opcode_mat[0x1][0x3] = [this]() { set_DE(get_DE() + 1); };
	m_opcode_mat[0x2][0x3] = [this]() { set_HL(get_HL() + 1); };
	m_opcode_mat[0x3][0x3] = [this]() { m_sp++; };

	m_opcode_mat[0x0][0xb] = [this]() { set_BC(get_BC() - 1); };
	m_opcode_mat[0x1][0xb] = [this]() { set_DE(get_DE() - 1); };
	m_opcode_mat[0x2][0xb] = [this]() { set_HL(get_HL() - 1); };
	m_opcode_mat[0x3][0xb] = [this]() { set_BC(get_BC() - 1); };

	m_opcode_mat[0x0][0x9] = [this]() { x16_alu_add(get_BC()); };
	m_opcode_mat[0x1][0x9] = [this]() { x16_alu_add(get_DE()); };
	m_opcode_mat[0x2][0x9] = [this]() { x16_alu_add(get_HL()); };
	m_opcode_mat[0x3][0x9] = [this]() { x16_alu_add(m_sp); };

	m_opcode_mat[0xe][0x8] = [this]() { x16_alu_addsp(); };
	m_opcode_mat[0xf][0x8] = [this]() { x16_alu_addsp(); set_HL(m_sp); };
};

void Gb_core::build_16bit_loads(){
	// 0x120d;
	m_opcode_mat[row(0xc1)][col(0xc1)] = [this](){ set_B(stack_pop()); set_C(stack_pop()); };
	m_opcode_mat[row(0xd1)][col(0xd1)] = [this](){ set_D(stack_pop()); set_E(stack_pop()); };
	m_opcode_mat[row(0xe1)][col(0xe1)] = [this](){ set_H(stack_pop()); set_L(stack_pop()); };
	m_opcode_mat[row(0xf1)][col(0xf1)] = [this](){ set_A(stack_pop()); set_F(stack_pop()); };
	m_opcode_mat[row(0xc5)][col(0xc5)] = [this](){ stack_push(get_C()); stack_push(get_B()); };
	m_opcode_mat[row(0xd5)][col(0xd5)] = [this](){ stack_push(get_E()); stack_push(get_D()); };
	m_opcode_mat[row(0xe5)][col(0xe5)] = [this](){ stack_push(get_L()); stack_push(get_H()); };
	m_opcode_mat[row(0xf5)][col(0xf5)] = [this](){ stack_push(get_F()); stack_push(get_A()); };
	m_opcode_mat[0x0][0x1] = [this](){ set_BC(pc_get_word()); };
	m_opcode_mat[0x1][0x1] = [this](){ set_DE(pc_get_word()); };
	m_opcode_mat[0x2][0x1] = [this](){ set_HL(pc_get_word()); };
	m_opcode_mat[0x3][0x1] = [this](){ m_sp = pc_get_word(); };
};

void Gb_core::build_8bit_loads(){
	for(int opcode = 0x40; opcode <= 0x47; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_B(m_reg_rmap[opcode % 8]());
		};
	};

	for(int opcode = 0x48; opcode <= 0x4f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_C(m_reg_rmap[opcode % 8]());
		};
	};

	for(int opcode = 0x50; opcode <= 0x57; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_D(m_reg_rmap[opcode % 8]());
		};
	};

	for(int opcode = 0x58; opcode <= 0x5f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_E(m_reg_rmap[opcode % 8]());
		};
	};

	for(int opcode = 0x60; opcode <= 0x67; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_H(m_reg_rmap[opcode % 8]());
		};
	}

	for(int opcode = 0x68; opcode <= 0x6f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_L(m_reg_rmap[opcode % 8]());
		};
	}

	for(int opcode = 0x70; opcode <= 0x77; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			m_memory->write(get_HL(), m_reg_rmap[opcode % 8]());
		};
	}

	for(int opcode = 0x78; opcode <= 0x7f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_A(m_reg_rmap[opcode % 8]());
		};
	}

	// 0xff00 loads
	m_opcode_mat[0xf][0x0] = [this](){ set_A(m_memory->read(0xff00 + pc_get_byte()));};
	m_opcode_mat[0xe][0x0] = [this](){ m_memory->write(0xff00 + pc_get_byte(), get_A());};
	m_opcode_mat[0xe][0x2] = [this](){ m_memory->write(0xff00 + get_C(), get_A()); };
	m_opcode_mat[0xf][0x2] = [this](){ set_A(m_memory->read(0xff00 + get_C())); };

	// ld [xx], A
	m_opcode_mat[0x0][0x2] = [this](){ m_memory->write(get_BC(), get_A()); };
	m_opcode_mat[0x1][0x2] = [this](){ m_memory->write(get_DE(), get_A()); };

	m_opcode_mat[0x2][0x2] = [this](){ 
		m_memory->write(get_HL(), get_A());
		set_HL(get_HL() + 1);
	};

	m_opcode_mat[0x3][0x2] = [this](){
		m_memory->write(get_HL(), get_A());
		set_HL(get_HL() - 1);
	};

	m_opcode_mat[0xe][0xa] = [this](){
		WORD addr = pc_get_word();
		m_memory->write(addr, get_A());
	};

	m_opcode_mat[0x0][0xa] = [this](){ set_A(m_memory->read(get_BC())); };
	m_opcode_mat[0x1][0xa] = [this](){ set_A(m_memory->read(get_DE())); };

	m_opcode_mat[0x2][0xa] = [this](){
		set_A(m_memory->read(get_HL()));
		set_HL(get_HL() + 1);
	};

	m_opcode_mat[0x3][0xa] = [this](){
		set_A(m_memory->read(get_HL()));
		set_HL(get_HL() + 1);
	};

	m_opcode_mat[0x0][0x6] = [this](){ set_B(pc_get_byte()); };
	m_opcode_mat[0x1][0x6] = [this](){ set_D(pc_get_byte()); };
	m_opcode_mat[0x2][0x6] = [this](){ set_H(pc_get_byte()); };
	m_opcode_mat[0x0][0xe] = [this](){ set_C(pc_get_byte()); };
	m_opcode_mat[0x1][0xe] = [this](){ set_E(pc_get_byte()); };
	m_opcode_mat[0x2][0xe] = [this](){ set_L(pc_get_byte()); };
	m_opcode_mat[0x3][0xe] = [this](){ set_A(pc_get_byte()); };
	m_opcode_mat[0x3][0x6] = [this](){ m_memory->write(get_HL(), pc_get_byte()); };
	m_opcode_mat[0xf][0xa] = [this](){ set_A(m_memory->read(pc_get_word())); };

};

void Gb_core::build_alu(){
	// opcodes mappings for subgrid [0x80, 0xbf]
	for(int j = 0; j <= 7; j++){
		m_opcode_mat[8][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_add(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[8][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_adc(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[9][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_sub(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[9][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_sbc(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[0xa][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_and(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[0xa][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_xor(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[0xb][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_or(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[0xb][j] = [this, j](){
			auto r2 = m_reg_rmap[static_cast<reg_order>(j % 8)]();
			x8_alu_cp(r2);
		};
	};

	// incs and decs mappings
	for(int opcode = 0x04; opcode <= 0x34; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this, opcode](){
			x8_alu_inc(static_cast<reg_order>((row(opcode) * 2)));
		};
	}

	for(int opcode = 0x0c; opcode <= 0x3c; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this, opcode](){
			x8_alu_inc(static_cast<reg_order>((row(opcode) * 2) + 1));
		};
	}

	for(int opcode = 0x05; opcode <= 0x35; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this, opcode](){
			x8_alu_dec(static_cast<reg_order>((row(opcode) * 2)));
		};
	}

	for(int opcode = 0x0d; opcode <= 0x3d; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this, opcode](){
			x8_alu_dec(static_cast<reg_order>((row(opcode) * 2) + 1));
		};
	}

	// alu misc opcode mappings
	m_opcode_mat[0xc][0x6] = [this](){ x8_alu_add(pc_get_byte()); };
	m_opcode_mat[0xd][0x6] = [this](){ x8_alu_sub(pc_get_byte()); };
	m_opcode_mat[0xe][0x6] = [this](){ x8_alu_and(pc_get_byte()); };
	m_opcode_mat[0xf][0x6] = [this](){ x8_alu_or(pc_get_byte()); };

	m_opcode_mat[0xc][0xe] = [this](){ x8_alu_adc(pc_get_byte()); };
	m_opcode_mat[0xd][0xe] = [this](){ x8_alu_sbc(pc_get_byte()); };
	m_opcode_mat[0xe][0xe] = [this](){ x8_alu_xor(pc_get_byte()); };
	m_opcode_mat[0xf][0xe] = [this](){ x8_alu_cp(pc_get_byte()); };

	m_opcode_mat[0x2][0x7] = [this](){ x8_alu_daa(); };
	m_opcode_mat[0x3][0x7] = [this](){ x8_alu_scf(); };
	m_opcode_mat[0x2][0xf] = [this](){ x8_alu_cpl(); };
	m_opcode_mat[0x3][0xf] = [this](){ x8_alu_ccf(); };
};

void Gb_core::build_registers_rmap(){
	m_reg_rmap[reg_order::REG_B] = [this]()->BYTE{ return get_B(); };
	m_reg_rmap[reg_order::REG_C] = [this]()->BYTE{ return get_C(); };

	m_reg_rmap[reg_order::REG_D] = [this]()->BYTE{ return get_D(); };
	m_reg_rmap[reg_order::REG_E] = [this]()->BYTE{ return get_E(); };

	m_reg_rmap[reg_order::REG_H] = [this]()->BYTE{ return get_H(); };
	m_reg_rmap[reg_order::REG_L] = [this]()->BYTE{ return get_L(); };

	m_reg_rmap[reg_order::REG_HL] = [this]() ->BYTE{ return m_memory->read(get_HL()); };
	m_reg_rmap[reg_order::REG_A] = [this]()->BYTE{ return get_A(); };
};

void Gb_core::build_registers_wmap(){
	// write map
	m_reg_wmap[reg_order::REG_B] = [this](BYTE v)->BYTE{ set_B(v); return get_B(); };
	m_reg_wmap[reg_order::REG_C] = [this](BYTE v)->BYTE{ set_C(v); return get_C(); };
	m_reg_wmap[reg_order::REG_D] = [this](BYTE v)->BYTE{ set_D(v); return get_D(); };

	m_reg_wmap[reg_order::REG_E] = [this](BYTE v)->BYTE{ set_E(v); return get_E(); };
	m_reg_wmap[reg_order::REG_H] = [this](BYTE v)->BYTE{ set_H(v); return get_H(); };
	m_reg_wmap[reg_order::REG_L] = [this](BYTE v)->BYTE{ set_L(v); return get_L(); };
	m_reg_wmap[reg_order::REG_HL] = [this](BYTE v)->BYTE{
		m_memory->write(get_HL(), v);
		return m_memory->read(get_HL());
	};
	m_reg_wmap[reg_order::REG_A] = [this](BYTE v)->BYTE{ set_A(v); return get_A(); };
};

void Gb_core::init_registers(){
	memset(m_registers, 0, sizeof(WORD) * 4);

	set_flag(flag::CARRY, true);
	set_flag(flag::HALF_CARRY, true);
	set_flag(flag::ZERO, true);
	set_A(0x01);
	set_BC(0x0013);
	set_DE(0x00d8);
	set_HL(0x014d);
	m_sp = SP_INIT_ADDR;
};

void Gb_core::build_cb_mat(){
	// srl
	m_cb_mat[0x3][0x8] = [this](){ set_B(srl(get_B())); };
	m_cb_mat[0x3][0x9] = [this](){ set_C(srl(get_C())); };
	m_cb_mat[0x3][0xa] = [this](){ set_D(srl(get_D())); };
	m_cb_mat[0x3][0xb] = [this](){ set_E(srl(get_E())); };
	m_cb_mat[0x3][0xc] = [this](){ set_H(srl(get_H())); };
	m_cb_mat[0x3][0xd] = [this](){ set_L(srl(get_L())); };
	m_cb_mat[0x3][0xe] = [this](){ 
		auto res = srl(m_memory->read(get_HL()));
		m_memory->write(get_HL(), res);
	};
	m_cb_mat[0x3][0xf] = [this](){ set_A(srl(get_A())); };

	// rr
	//
	m_cb_mat[0x1][0x8] = [this](){ set_B(rr(get_B())); };
	m_cb_mat[0x1][0x9] = [this](){ set_C(rr(get_C())); };
	m_cb_mat[0x1][0xa] = [this](){ set_D(rr(get_D())); };
	m_cb_mat[0x1][0xb] = [this](){ set_E(rr(get_E())); };
	m_cb_mat[0x1][0xc] = [this](){ set_H(rr(get_H())); };
	m_cb_mat[0x1][0xd] = [this](){ set_L(rr(get_L())); };
	m_cb_mat[0x1][0xe] = [this](){
		auto res = rr(m_memory->read(get_HL()));
		m_memory->write(get_HL(), res);
	};
	m_cb_mat[0x1][0xf] = [this](){ set_A(rr(get_A())); };

};

void Gb_core::init(){
	build_alu();
	build_control();
	build_registers_rmap();
	build_registers_wmap();
	build_8bit_loads();
	build_16bit_loads();
	build_cb_mat();
	init_registers();


	m_intrp_addr[intrp::VBLANK] = 0x40;
	m_intrp_addr[intrp::LCD] = 0x48;
	m_intrp_addr[intrp::TIMER] = 0x50;
	m_intrp_addr[intrp::SERIAL] = 0x58;
	m_intrp_addr[intrp::JOYPAD] = 0x60;

	m_opcode_mat[0xc][0xb] = [this](){ // forward opcode to m_cb_mat
		auto opcode = pc_get_byte();
		auto op_handler = m_cb_mat[ROW(opcode)][COL(opcode)];
		if(op_handler != nullptr){
			op_handler();
		}else{
			std::cerr << "Unknown cb opcode : " << std::hex << (int)opcode << std::endl;
			exit(1);
		}
	};

	// rra
	m_opcode_mat[0x1][0xf] = [this](){ set_A(rr(get_A())); };
};

// real cycles not taken into account for now.
void Gb_core::emulate_cycles(int n){
	for(int i = 0; i < n; i++){
		//log();
		auto opcode = pc_get_byte();
		auto opcode_handler = m_opcode_mat[ROW(opcode)][COL(opcode)];
		if(opcode_handler != nullptr){
			opcode_handler();
		}else{
			std::cerr << "Unknown opcode : " << std::hex << (int)opcode << std::endl;
			exit(1);
		}
	}
};

void Gb_core::jmp_nn(){ m_pc = pc_get_word();};

BYTE Gb_core::stack_pop(){ return m_memory->read(++m_sp); };

void Gb_core::stack_push(BYTE value){ m_memory->write(m_sp--, value); };

void Gb_core::set_flag(Gb_core::flag f, bool set){
	set_F(set ? (get_F() | (0x1 << f)) : (get_F() & ~(0x1 << f)));
};

constexpr BYTE Gb_core::get_flag(flag f){ return (get_F() >> f) & 0x1; };
// ---- ----
void Gb_core::x8_alu_add(BYTE r2){
	BYTE r1 = get_A();

	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, (((r1 & 0x0f) + (r2 + 0x0f)) & 0x10));
	set_flag(flag::CARRY, (static_cast<WORD>(r1) + static_cast<WORD>(r2) & 0x100));
	set_flag(flag::ZERO, static_cast<BYTE>(r1 + r2) == 0);

	set_A(r1 + r2);
};

void Gb_core::x8_alu_adc(BYTE r2){
	BYTE r1 = get_A();
	BYTE c_flag = get_flag(flag::CARRY);

	set_flag(flag::HALF_CARRY, (((r1 & 0x0f) + (r2 + 0x0f) + c_flag) & 0x10));
	set_flag(flag::CARRY, (static_cast<WORD>(r1) + static_cast<WORD>(r2) + c_flag > 0xff));
	set_flag(flag::ZERO, r1 + r2 + c_flag == 0);
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
	set_flag(flag::CARRY, (static_cast<int>(r1 & 0x0f) < (static_cast<int>(r2 & 0x0f) + c_flag)));

	set_flag(flag::CARRY, r1 < (r2 + c_flag));

	set_flag(flag::ZERO, (r1 - (r2 + c_flag) == 0));

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
};

void Gb_core::x8_alu_inc(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r + 1);

	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x00));
};

void Gb_core::x8_alu_dec(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r - 1);
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, true);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x0f));
};

void Gb_core::x8_alu_daa(){
	BYTE A = get_A();
	BYTE res, correction = 0;

	if(get_flag(flag::HALF_CARRY) || (!get_flag(flag::SUBS) && (A & 0xf) > 9)){
		correction |= 0x06;
	};

	if(get_flag(flag::CARRY) || (!get_flag(flag::SUBS) && (A > 0x99))){
		correction |= 0x60;
	};

	if(get_flag(flag::SUBS)){
		set_flag(flag::CARRY, (static_cast<WORD>(A - correction) & 0x100));
		res = A - correction;
	}else{
		set_flag(flag::CARRY, (static_cast<WORD>(A + correction) & 0x100));
		res = A + correction;
	}
	
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, false);

	set_A(res);
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

void Gb_core::ctrl_return(){
	set_lower(m_pc, stack_pop());	
	set_upper(m_pc, stack_pop());	
};

void Gb_core::ctrl_call(bool cond){
	if(cond){
		BYTE upper = pc_get_byte();
		BYTE lower = pc_get_byte();

		stack_push(get_upper(m_pc));
		stack_push(get_lower(m_pc));

		WORD jmp_addr = upper | (lower << 8);
		m_pc = jmp_addr;
	}else{
		m_pc += 2;
	}

};

void Gb_core::ctrl_jr(bool cond){
	if(cond){
		auto offset = static_cast<SIGNED_BYTE>(pc_get_byte());
		m_pc += offset;
	}else{
		m_pc += 1;
	}
};

void Gb_core::ctrl_rst(const WORD offset){
	stack_push(m_pc);
	m_pc = offset;
};

void Gb_core::x16_alu_inc(WORD& rr){ rr++; };
void Gb_core::x16_alu_dec(WORD& rr){ rr++; };

void Gb_core::x16_alu_add(WORD rr){
	set_flag(flag::SUBS, false);
	auto hl = get_HL();
	hl += rr;

	set_flag(flag::CARRY, ((static_cast<unsigned int>(hl) + static_cast<unsigned int>(rr)) & 0x10000));
	set_flag(flag::HALF_CARRY, (((hl & 0xfff) + (rr & 0xfff)) & 0x1000));

	set_HL(hl);
};

void Gb_core::x16_alu_addsp(){
	SIGNED_BYTE value = static_cast<SIGNED_BYTE>(pc_get_byte());

	set_flag(flag::ZERO, false);
	set_flag(flag::SUBS, false);

	set_flag(flag::HALF_CARRY, (((m_sp & 0xf) + (value & 0xf)) & 0x10));
	set_flag(flag::CARRY, (((m_sp & 0xff) + (value & 0xff)) & 0x100));

	m_sp += value;
};

WORD Gb_core::pc_get_word(){ 
	WORD value = (m_memory->read(m_pc + 1) << 8) | (m_memory->read(m_pc));
	m_pc += 2;
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

	set_flag(flag::CARRY, _0bit);

	r = (r >> 1) | (_0bit << 7);

	set_flag(flag::ZERO, r == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, false);

	return r;
};

void Gb_core::call_interrupt(intrp i){
	// two waits are executed first
	// prevent further interrupts
	m_interrupts_enabled = false;

	// acknoledge interrupt
	auto intrp_flag = m_memory->read(IF_ADDR);
	intrp_flag &= ~(0x1 << i);
	m_memory->write(IF_ADDR, intrp_flag);

	stack_push(get_lower(m_pc));
	stack_push(get_upper(m_pc));

	// call interrupt handler
	assert(m_intrp_addr.find(i) != m_intrp_addr.end());
	m_pc = m_intrp_addr[i];
};

void Gb_core::handle_interrupts(){
	if(m_interrupts_enabled){
		auto intrp_flag = m_memory->read(IF_ADDR);
		auto intrp_enbaled = m_memory->read(IE_ADDR);
		if(intrp_flag == 0) return;
		for(int i = 0; i < 5; i++){
			if(((intrp_flag >> i) & 0x1) && ((intrp_enbaled >> i) & 0x1)){
				call_interrupt(static_cast<intrp>(i));
			}
		};

	};
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
	//printf("op: %02hhX)\n", (unsigned int)tmp);
};
