#include "Gb_core.h"
#include <variant>
#include <stdio.h>

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {};

void Gb_core::build_control(){
	// returns
	m_opcode_mat[0x0][0x0] = [this](){ m_pc++; };
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
	m_opcode_mat[0xf][0x3] = [this](){ m_interrupts_enabled = false; m_pc++; };
	m_opcode_mat[0xf][0xb] = [this](){ m_interrupts_enabled = true; m_pc++; };
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
	m_opcode_mat[0x0][0x3] = [this]() { set_BC(get_BC() + 1); m_pc++; };
	m_opcode_mat[0x1][0x3] = [this]() { set_DE(get_DE() + 1); m_pc++; };
	m_opcode_mat[0x2][0x3] = [this]() { set_HL(get_HL() + 1); m_pc++; };
	m_opcode_mat[0x3][0x3] = [this]() { m_sp++; m_pc++;};

	m_opcode_mat[0x0][0xb] = [this]() { set_BC(get_BC() - 1); m_pc++; };
	m_opcode_mat[0x1][0xb] = [this]() { set_DE(get_DE() - 1); m_pc++; };
	m_opcode_mat[0x2][0xb] = [this]() { set_HL(get_HL() - 1); m_pc++; };
	m_opcode_mat[0x3][0xb] = [this]() { set_BC(get_BC() - 1); m_pc++; };

	m_opcode_mat[0x0][0x9] = [this]() { x16_alu_add(get_BC()); };
	m_opcode_mat[0x1][0x9] = [this]() { x16_alu_add(get_DE()); };
	m_opcode_mat[0x2][0x9] = [this]() { x16_alu_add(get_HL()); };
	m_opcode_mat[0x3][0x9] = [this]() { x16_alu_add(m_sp); };

	m_opcode_mat[0xe][0x8] = [this]() { x16_alu_addsp(); };
	m_opcode_mat[0xf][0x8] = [this]() { x16_alu_addsp(); set_HL(m_sp); m_pc++; };
};

void Gb_core::build_16bit_loads(){

	// 0x120d;
	m_opcode_mat[row(0xc1)][col(0xc1)] = [this]() { 
		set_BC(stack_pop() | (stack_pop() << 8));
		m_pc++;
	};

	m_opcode_mat[row(0xd1)][col(0xd1)] = [this](){ 
		set_DE(stack_pop() | (stack_pop() << 8));
		m_pc++;
	};
	m_opcode_mat[row(0xe1)][col(0xe1)] = [this](){ 
		set_HL(stack_pop() | (stack_pop() << 8));
		m_pc++;
	};
	m_opcode_mat[row(0xf1)][col(0xf1)] = [this](){ 
		// what about fff??
		set_AF(stack_pop() | (stack_pop() << 8));
		m_pc++;
	};

	m_opcode_mat[row(0xc5)][col(0xc5)] = [this](){ 
		stack_push(get_C());
		stack_push(get_B());
		m_pc++;
	};

	m_opcode_mat[row(0xd5)][col(0xd5)] = [this](){ 
		stack_push(get_E());
		stack_push(get_D());
		m_pc++;
	};

	m_opcode_mat[row(0xe5)][col(0xe5)] = [this](){ 
		stack_push(get_L());
		stack_push(get_H());
		m_pc++;
	};
	m_opcode_mat[row(0xf5)][col(0xf5)] = [this](){ 
		stack_push(get_F());
		stack_push(get_A());
		m_pc++;
	};
};

void Gb_core::build_8bit_loads(){
	for(int opcode = 0x40; opcode <= 0x47; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_B(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	};

	for(int opcode = 0x48; opcode <= 0x4f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_C(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	};

	for(int opcode = 0x50; opcode <= 0x47; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_D(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	};

	for(int opcode = 0x58; opcode <= 0x5f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_E(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	};

	for(int opcode = 0x60; opcode <= 0x67; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_H(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	}

	for(int opcode = 0x68; opcode <= 0x6f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_L(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	}

	for(int opcode = 0x70; opcode <= 0x77; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			m_memory->write(get_HL(), m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	}

	for(int opcode = 0x78; opcode <= 0x7f; opcode++){
		m_opcode_mat[row(opcode)][col(opcode)] = [this, opcode](){ 
			set_A(m_reg_rmap[opcode % 8]());
			m_pc++;
		};
	}


};

void Gb_core::build_alu_x80_xbf(){
	for(int j = 0; j <= 7; j++){
		m_opcode_mat[8][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_add(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[8][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_adc(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[9][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_sub(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[9][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_sbc(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[0xa][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_and(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[0xa][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_xor(r2);
		};
	};

	for(int j = 0; j <= 7; j++){
		m_opcode_mat[0xb][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_or(r2);
		};
	};

	for(int j = 8; j <= 0xf; j++){
		m_opcode_mat[0xb][j] = [this](){
			auto opcode = m_memory->read(m_pc);
			auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
			x8_alu_cp(r2);
		};
	};


};

void Gb_core::build_alu_inc_dec(){
	for(int opcode = 0x04; opcode <= 0x34; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){
			auto opcode = m_memory->read(m_pc);
			x8_alu_inc(static_cast<reg_order>((row(opcode) * 2)));
		};
	}

	for(int opcode = 0x0c; opcode <= 0x3c; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){
			auto opcode = m_memory->read(m_pc);
			x8_alu_inc(static_cast<reg_order>((row(opcode) * 2) + 1));
		};
	}

	for(int opcode = 0x05; opcode <= 0x35; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){
			auto opcode = m_memory->read(m_pc);
			x8_alu_dec(static_cast<reg_order>((row(opcode) * 2)));
		};
	}

	for(int opcode = 0x0d; opcode <= 0x3d; opcode += 0x10){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){
			auto opcode = m_memory->read(m_pc);
			x8_alu_dec(static_cast<reg_order>((row(opcode) * 2) + 1));
		};
	}
};

void Gb_core::build_opcode_matrix(){

	for(auto opcode : opcodes_8bitld_u8()){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){ _8bit_ldu8(); };
	};

	for(auto opcode : opcodes_8bitld_XX_R()){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){ _8bit_ld_xxA(); };
	};

	for(auto opcode : opcodes_8bitld_Axx()){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this]() { _8bit_ld_Axx(); };
	};

	for(auto opcode : opcodes_16bitld_u16()){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){ _16_bit_ld(); };
	};

	for(auto opcode : opcodes_ff00()){
		m_opcode_mat[ROW(opcode)][COL(opcode)] = [this](){ _8bit_ld_ff00(); };
	};

	// alu
	m_opcode_mat[0xc][0x6] = [this](){ x8_alu_add(m_memory->read(m_pc++)); };
	m_opcode_mat[0xd][0x6] = [this](){ x8_alu_sub(m_memory->read(m_pc++)); };
	m_opcode_mat[0xe][0x6] = [this](){ x8_alu_and(m_memory->read(m_pc++)); };
	m_opcode_mat[0xf][0x6] = [this](){ x8_alu_or(m_memory->read(m_pc++)); };

	m_opcode_mat[0xc][0xe] = [this](){ x8_alu_adc(m_memory->read(m_pc++)); };
	m_opcode_mat[0xd][0xe] = [this](){ x8_alu_sbc(m_memory->read(m_pc++)); };
	m_opcode_mat[0xe][0xe] = [this](){ x8_alu_xor(m_memory->read(m_pc++)); };
	m_opcode_mat[0xf][0xe] = [this](){ x8_alu_cp(m_memory->read(m_pc++)); };

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

void Gb_core::init(){
	build_opcode_matrix();
	build_alu_x80_xbf();
	build_alu_inc_dec();
	build_control();
	build_registers_rmap();
	build_registers_wmap();
	build_8bit_loads();
	build_16bit_loads();
	init_registers();
};

// real cycles not taken into account for now.
void Gb_core::emulate_cycles(int n){
	for(int i = 0; i < n; i++){
		log();
		auto opcode = m_memory->read(m_pc);
		auto opcode_handler = m_opcode_mat[ROW(opcode)][COL(opcode)];
		if(opcode_handler != nullptr){
			opcode_handler();
		}else{
			std::cerr << "Unknown opcode : " << std::hex << (int)opcode << std::endl;
			exit(1);
		}
	}
};

int Gb_core::_8bit_load(BYTE& rg, BYTE value){
	rg = value;
	return LD_8BIT_CYCLES;
};

int Gb_core::ld_addr_r(WORD addr, BYTE r){
	switch(r){
		case Gb_core::reg_order::REG_B: m_memory->write(addr, get_B()); break;
		case Gb_core::reg_order::REG_C: m_memory->write(addr, get_C()); break;
		case Gb_core::reg_order::REG_D: m_memory->write(addr, get_D()); break;
		case Gb_core::reg_order::REG_E: m_memory->write(addr, get_E()); break;
		case Gb_core::reg_order::REG_H: m_memory->write(addr, get_H()); break;
		case Gb_core::reg_order::REG_L: m_memory->write(addr, get_L()); break;
		default: break;
	};
	return 8;
};

void Gb_core::jmp_nn(){
	m_pc++;
	WORD nn = (m_memory->read(m_pc + 1) << 8) | m_memory->read(m_pc);
	m_pc = nn;
};

void Gb_core::_8bit_ldu8(){
	auto opcode = m_memory->read(m_pc);
	if((opcode & 0x0f) == 0x6){
		switch((opcode & 0xf0) >> 4){
			case 0: set_B(m_memory->read(m_pc + 1)); break;
			case 1: set_D(m_memory->read(m_pc + 1)); break;
			case 2: set_H(m_memory->read(m_pc + 1)); break;
			case 3: m_memory->write(get_HL(), m_memory->read(m_pc + 1)); break;
		}
	}else if((opcode & 0x0f) == 0xe){
		switch((opcode & 0xf0) >> 4){
			case 0: set_C(m_memory->read(m_pc + 1)); break;
			case 1: set_E(m_memory->read(m_pc + 1)); break;
			case 2: set_L(m_memory->read(m_pc + 1)); break;
			case 3: set_A(m_memory->read(m_pc + 1)); break;
		}
	}else std::cerr << "Uknown opcode : " << (int)opcode << std::endl;

	m_pc += 2;
};

void Gb_core::_8bit_ld_xxA(){
	auto opcode = m_memory->read(m_pc);
	WORD value, addr;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::_BC_A: m_memory->write(get_BC(), get_A()); break;
		case ld_8bit::_DE_A: m_memory->write(get_DE(), get_A()); break;
		case ld_8bit::_HL_INC_A:
			m_memory->write(get_HL(), get_A());
			set_HL(get_HL() + 1);
			break;
		case ld_8bit::_HL_DEC_A:
			m_memory->write(get_HL(), get_A());
			set_HL(get_HL() - 1);
			break;
		case ld_8bit::_U16_A:
			addr = (m_memory->read(m_pc + 2) << 8)  | m_memory->read(m_pc + 1);
			m_memory->write(addr, get_A());
			m_pc += 2;
			break;
		default: std::cerr << "here Uknown opcode : " << (int)opcode << std::endl;
	}
	m_pc++;
};
		
void Gb_core::_8bit_ld_Axx(){
	auto opcode = m_memory->read(m_pc);
	WORD addr;
	BYTE value;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::A_BC_: set_A(m_memory->read(get_BC())); break;
		case ld_8bit::A_DE_: set_A(m_memory->read(get_DE())); break;
		case ld_8bit::A_HL_INC:
			set_A(m_memory->read(get_HL()));
			set_HL(get_HL() + 1);
			break;
		case ld_8bit::A_HL_DEC:
			set_A(m_memory->read(get_HL()));
			set_HL(get_HL() + 1);
			break;
		case ld_8bit::A_U16_:
			addr = (m_memory->read(m_pc + 2) << 8)  | m_memory->read(m_pc + 1);
			set_A(m_memory->read(addr));
			m_pc += 2;
			break;
		default: std::cerr << "here Uknown opcode : " << (int)opcode << std::endl;
	}
	m_pc++;
};

void Gb_core::_16_bit_ld(){
	auto opcode = m_memory->read(m_pc);
	WORD value = (m_memory->read(m_pc + 2) << 8) | (m_memory->read(m_pc + 1));
	switch(static_cast<ld_16bit>(opcode)){
		case ld_16bit::BC_U16: set_BC(value); break;
		case ld_16bit::DE_U16: set_DE(value); break;
		case ld_16bit::HL_U16: set_HL(value); break;
		case ld_16bit::SP_U16: m_sp = value; break;
		default:
			std::cerr << "Uknown opcode " << std::hex << (int)opcode << std::endl;
			break;
	};
	m_pc += 3;
};

std::vector<Gb_core::ld_8bit> Gb_core::opcodes_8bitld_u8() const{
	return {ld_8bit::B_U8, ld_8bit::D_U8, ld_8bit::H_U8, ld_8bit::A_U8,
			ld_8bit::HL_U8,ld_8bit::C_U8, ld_8bit::E_U8, ld_8bit::L_U8};
}

std::vector<Gb_core::ld_8bit> Gb_core::opcodes_8bitld_XX_R() const{
	return {ld_8bit::_BC_A, ld_8bit::_DE_A, ld_8bit::_HL_INC_A, ld_8bit::_HL_DEC_A, ld_8bit::_U16_A};
};

std::vector<Gb_core::ld_8bit> Gb_core::opcodes_8bitld_Axx() const{
	return {ld_8bit::A_BC_, ld_8bit::A_DE_, ld_8bit::A_HL_INC, ld_8bit::A_HL_DEC, ld_8bit::A_U16_};
}

std::vector<Gb_core::ld_16bit> Gb_core::opcodes_16bitld_u16() const{
	return {ld_16bit::BC_U16, ld_16bit::DE_U16, ld_16bit::HL_U16, ld_16bit::SP_U16};
}

std::vector<Gb_core::ld_8bit> Gb_core::opcodes_ff00() const {
	return {ld_8bit::_FF00_U8_A, ld_8bit::A_FF00_U8_, ld_8bit::_FF00_C_A,
			ld_8bit::A_FF00_C_};
}

std::vector<Gb_core::ld_16bit> Gb_core::opcodes_16bitld_stack() const{
	return {ld_16bit::POP_BC, ld_16bit::PUSH_BC, ld_16bit::PUSH_DE, ld_16bit::POP_DE,
			ld_16bit::POP_HL, ld_16bit::PUSH_HL, ld_16bit::POP_AF, ld_16bit::PUSH_AF};
};

void Gb_core::_8bit_ld_ff00(){
	auto opcode = m_memory->read(m_pc);
	WORD offset;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::_FF00_U8_A:
			offset = m_memory->read(m_pc + 1);
			m_memory->write(0xff00 + offset, get_A());
			m_pc++;
			break;
		case ld_8bit::A_FF00_U8_:
			offset = m_memory->read(m_pc + 1);
			set_A(m_memory->read(0xff00 + offset));
			m_pc++;
			break;
		case ld_8bit::_FF00_C_A:
			m_memory->write(0xff00 + get_C(), get_A());
			break;
		case ld_8bit::A_FF00_C_:
			set_A(m_memory->read(0xff00 + get_C()));
			break;
		default:
			std::cerr << "Uknown opcode : " << std::hex << (int)opcode << std::endl;
	};
	m_pc++;
};

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
	m_pc++;
};

void Gb_core::x8_alu_adc(BYTE r2){
	BYTE r1 = get_A();
	BYTE c_flag = get_flag(flag::CARRY);

	set_flag(flag::HALF_CARRY, (((r1 & 0x0f) + (r2 + 0x0f) + c_flag) & 0x10));
	set_flag(flag::CARRY, (static_cast<WORD>(r1) + static_cast<WORD>(r2) + c_flag > 0xff));
	set_flag(flag::ZERO, r1 + r2 + c_flag == 0);
	set_A(r1 + r2 + c_flag);

	m_pc++;
};

void Gb_core::x8_alu_sub(BYTE r2){
	BYTE r1 = get_A();

	set_flag(flag::SUBS, true);
	set_flag(flag::ZERO, r1 - r2 == 0);
	set_flag(flag::HALF_CARRY, (static_cast<int>(r1 & 0x0f) < static_cast<int>(r2 & 0x0f)));
	set_flag(flag::CARRY, r1 < r2);

	set_A(r1 - r2);
	m_pc++;
};

void Gb_core::x8_alu_sbc(BYTE r2){
	BYTE r1 = get_A();
	set_flag(flag::SUBS, true);

	BYTE c_flag = get_flag(flag::CARRY);
	set_flag(flag::CARRY, (static_cast<int>(r1 & 0x0f) < (static_cast<int>(r2 & 0x0f) + c_flag)));

	set_flag(flag::CARRY, r1 < (r2 + c_flag));

	set_flag(flag::ZERO, (r1 - (r2 + c_flag) == 0));

	set_A(r1 - (r2 + c_flag));
	m_pc++;
};

void Gb_core::x8_alu_and(BYTE r2){
	auto res = get_A() & r2;
	
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, true);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
	m_pc++;
};

void Gb_core::x8_alu_xor(BYTE r2){
	auto res = get_A() ^ r2;
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
	m_pc++;
};

void Gb_core::x8_alu_or(BYTE r2){
	auto res = get_A() | r2;

	set_flag(flag::ZERO, res  == 0);
	set_flag(flag::HALF_CARRY, false);
	set_flag(flag::CARRY, false);
	set_flag(flag::SUBS, false);

	set_A(res);
	m_pc++;
};


void Gb_core::x8_alu_cp(BYTE r2){
	BYTE a = get_A();

	set_flag(flag::SUBS, true);
	set_flag(flag::ZERO, a == r2);
	set_flag(flag::HALF_CARRY, ((a & 0x0f) - (r2 & 0x0f) < 0));
	set_flag(flag::CARRY, a < r2);

	m_pc++;
};

void Gb_core::x8_alu_inc(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r + 1);

	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, false);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x00));

	m_pc++;
};

void Gb_core::x8_alu_dec(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r - 1);
	set_flag(flag::ZERO, res == 0);
	set_flag(flag::SUBS, true);
	set_flag(flag::HALF_CARRY, ((res & 0x0f) == 0x0f));
	m_pc++;
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

	m_pc++;
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
		m_pc++;
		BYTE upper = m_memory->read(m_pc++);
		BYTE lower = m_memory->read(m_pc++);

		stack_push(get_upper(m_pc));
		stack_push(get_lower(m_pc));

		WORD jmp_addr = upper | (lower << 8);
		m_pc = jmp_addr;
	}else{
		m_pc += 3;
	}

};

void Gb_core::ctrl_jr(bool cond){
	auto offset2 = static_cast<SIGNED_BYTE>(m_memory->read(m_pc + 1));
	if(cond){
		auto offset = static_cast<SIGNED_BYTE>(m_memory->read(m_pc + 1));
		m_pc += offset + 2;
	}else{
		m_pc += 2;
	}
};

void Gb_core::ctrl_rst(const WORD offset){
	stack_push(m_pc);
	m_pc = offset;
};

void Gb_core::x16_alu_inc(WORD& rr){ rr++; m_pc++;};
void Gb_core::x16_alu_dec(WORD& rr){ rr++; m_pc++;};

void Gb_core::x16_alu_add(WORD rr){
	set_flag(flag::SUBS, false);
	auto hl = get_HL();
	hl += rr;

	set_flag(flag::CARRY, ((static_cast<unsigned int>(hl) + static_cast<unsigned int>(rr)) & 0x10000));
	set_flag(flag::HALF_CARRY, (((hl & 0xfff) + (rr & 0xfff)) & 0x1000));

	set_HL(hl);
	m_pc++;
};

void Gb_core::x16_alu_addsp(){
	SIGNED_BYTE value = static_cast<SIGNED_BYTE>(m_memory->read(m_pc + 1));

	set_flag(flag::ZERO, false);
	set_flag(flag::SUBS, false);

	set_flag(flag::HALF_CARRY, (((m_sp & 0xf) + (value & 0xf)) & 0x10));
	set_flag(flag::CARRY, (((m_sp & 0xff) + (value & 0xff)) & 0x100));

	m_sp += value;
	m_pc += 2;
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
