#include "Gb_core.h"
#include <variant>
#include <stdio.h>

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {
}

void Gb_core::build_opcode_matrix(){

	// sub grid : 0x40 - 0x7f
	for(int row = 4; row <= 7; row++){
		for(int col = 0; col < 16; col++){
			BYTE opcode = (row << 4) | col;
			auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_8bit>(opcode),
														&Gb_core::_8bit_ld_r1r2, 8);
			m_opcode_mat[row][col] = ptr;
		}
		std::cout << std::endl;
	}

	for(auto opcode : opcodes_8bitld_u8()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_8bit>(opcode),
													&Gb_core::_8bit_ldu8, 8);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(auto opcode : opcodes_8bitld_XX_R()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_8bit>(opcode),
													&Gb_core::_8bit_ld_xxA, 8);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(auto opcode : opcodes_8bitld_Axx()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_8bit>(opcode),
													&Gb_core::_8bit_ld_Axx, 8);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(auto opcode : opcodes_16bitld_u16()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_16bit>(opcode),
													&Gb_core::_16_bit_ld, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(auto opcode : opcodes_ff00()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_8bit>(opcode),
													&Gb_core::_8bit_ld_ff00, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(auto opcode : opcodes_16bitld_stack()){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<ld_16bit>(opcode),
													&Gb_core::_16bit_ldsp, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	}

	for(auto opcode : opcodes_alu()){
		std::shared_ptr<Gb_instruction> ptr;
		ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	}

	for(int opcode = 0xb0; opcode <= 0xb7; opcode++){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(int opcode = 0xb8; opcode <= 0xbf; opcode++){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(int opcode = 0x04; opcode <= 0x34; opcode += 0x10){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	}

	for(int opcode = 0x0c; opcode <= 0x3c; opcode += 0x10){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	}

	for(int opcode = 0x05; opcode <= 0x35; opcode += 0x10){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};

	for(int opcode = 0x0d; opcode <= 0x3d; opcode += 0x10){
		auto ptr = std::make_shared<Gb_instruction>(static_cast<alu>(opcode),&Gb_core::core_alu, 12);
		m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
	};


	auto daa_ptr = std::make_shared<Gb_instruction>(static_cast<alu>(0x27),&Gb_core::x8_alu_daa, 12);
	m_opcode_mat[ROW(0x27)][COL(0x27)] = daa_ptr;

	auto ptr = std::make_shared<Gb_instruction>(i_control::JMP_NN, &Gb_core::jmp_nn, 16);
	auto opcode = static_cast<BYTE>(i_control::JMP_NN);
	m_opcode_mat[ROW(opcode)][COL(opcode)] = ptr;
};

void Gb_core::build_registers_rmap(){
	m_reg_rmap[reg_order::REG_B] = [this]()->BYTE{ return m_registerBC.hi; };
	m_reg_rmap[reg_order::REG_C] = [this]()->BYTE{ return m_registerBC.lo; };

	m_reg_rmap[reg_order::REG_D] = [this]()->BYTE{ return m_registerDE.hi; };
	m_reg_rmap[reg_order::REG_E] = [this]()->BYTE{ return m_registerDE.lo; };

	m_reg_rmap[reg_order::REG_H] = [this]()->BYTE{ return m_registerHL.hi; };
	m_reg_rmap[reg_order::REG_L] = [this]()->BYTE{ return m_registerHL.lo; };

	m_reg_rmap[reg_order::REG_HL] = [this]() -> BYTE{ return m_memory->read(m_registerHL.pair); };
	m_reg_rmap[reg_order::REG_A] = [this]()->BYTE{ return m_registerAF.hi; };
};

void Gb_core::build_registers_wmap(){
	// write map
	m_reg_wmap[reg_order::REG_B] = [this](BYTE v)->BYTE{ return(m_registerBC.hi = v);};
	m_reg_wmap[reg_order::REG_C] = [this](BYTE v)->BYTE{ return(m_registerBC.lo = v);};
	m_reg_wmap[reg_order::REG_D] = [this](BYTE v)->BYTE{ return(m_registerDE.hi = v);};
	m_reg_wmap[reg_order::REG_E] = [this](BYTE v)->BYTE{ return(m_registerDE.lo = v);};
	m_reg_wmap[reg_order::REG_H] = [this](BYTE v)->BYTE{ return(m_registerHL.hi = v);};
	m_reg_wmap[reg_order::REG_L] = [this](BYTE v)->BYTE{ return(m_registerHL.lo = v);};
	m_reg_wmap[reg_order::REG_HL] = [this](BYTE v)->BYTE{
		m_memory->write(m_registerHL.pair, v);
		return m_memory->read(m_registerHL.pair);
	};
	m_reg_wmap[reg_order::REG_A] = [this](BYTE v)->BYTE{ return(m_registerAF.hi = v);};
};

void Gb_core::init(){
	build_opcode_matrix();
	m_sp.pair = SP_INIT_ADDR;
	m_registerBC.pair = 0;
	m_registerAF.pair = 0;
	build_registers_rmap();
	build_registers_wmap();
	m_reg16_map[0] = [this]()->WORD& { return m_registerBC.pair; };
	m_reg16_map[1] = [this]()->WORD& { return m_registerDE.pair; };
	m_reg16_map[2] = [this]()->WORD& { return m_registerHL.pair; };
	m_reg16_map[3] = [this]()->WORD& { return m_registerAF.pair; };
};

// real cycles not taken into account for now.
void Gb_core::emulate_cycles(int n){
	for(int i = 0; i < n; i++){
		auto opcode = m_memory->read(m_pc);
		auto i_ptr = m_opcode_mat[ROW(opcode)][COL(opcode)];
		if(i_ptr.get() != nullptr){
			(this->*i_ptr->fn)(); // run instruction handler
		}else{
			std::cerr << "Unknown opcode : " << std::hex << (int)opcode << std::endl;
		}
	}
};

void Gb_core::_8bit_ld_r1r2(){
	auto opcode = static_cast<ld_8bit>(m_memory->read(m_pc));
	switch(opcode){
		case ld_8bit::A_A: case ld_8bit::A_B: case ld_8bit::A_C:
		case ld_8bit::A_D: case ld_8bit::A_E: case ld_8bit::A_H:
		case ld_8bit::A_L: case ld_8bit::A_HL:
			ld_r_v(m_registerAF.hi, (BYTE)opcode % 8);
			break;
		case ld_8bit::B_B: case ld_8bit::B_C: case ld_8bit::B_D:
		case ld_8bit::B_E: case ld_8bit::B_H: case ld_8bit::B_L:
		case ld_8bit::B_HL: case ld_8bit::B_A:
			ld_r_v(m_registerBC.hi, (BYTE)opcode % 8);
			break;
		case ld_8bit::C_B: case ld_8bit::C_C: case ld_8bit::C_D:
		case ld_8bit::C_E: case ld_8bit::C_H: case ld_8bit::C_L:
		case ld_8bit::C_HL: case ld_8bit::C_A:
			ld_r_v(m_registerBC.lo, (BYTE)opcode % 8);
			break;
		case ld_8bit::D_B: case ld_8bit::D_C: case ld_8bit::D_D:
		case ld_8bit::D_E: case ld_8bit::D_H: case ld_8bit::D_L:
		case ld_8bit::D_HL: case ld_8bit::D_A:
			ld_r_v(m_registerDE.hi, (BYTE)opcode % 8);
			break;
		case ld_8bit::E_B: case ld_8bit::E_C: case ld_8bit::E_D:
		case ld_8bit::E_E: case ld_8bit::E_H: case ld_8bit::E_L:
		case ld_8bit::E_HL: case ld_8bit::E_A:
			ld_r_v(m_registerDE.lo, (BYTE)opcode % 8);
			break;
		case ld_8bit::H_B: case ld_8bit::H_C: case ld_8bit::H_D:
		case ld_8bit::H_E: case ld_8bit::H_H: case ld_8bit::H_L:
		case ld_8bit::H_HL: case ld_8bit::H_A:
			ld_r_v(m_registerHL.hi, (BYTE)opcode % 8);
			break;
		case ld_8bit::L_B: case ld_8bit::L_C: case ld_8bit::L_D:
		case ld_8bit::L_E: case ld_8bit::L_H: case ld_8bit::L_L:
		case ld_8bit::L_HL: case ld_8bit::L_A:
			ld_r_v(m_registerHL.lo, (BYTE)opcode % 8);
			break;
		case ld_8bit::HL_B: case ld_8bit::HL_C: case ld_8bit::HL_D:
		case ld_8bit::HL_E: case ld_8bit::HL_H: case ld_8bit::HL_L:
		case ld_8bit::HL_A:
			ld_addr_r(m_registerHL.pair, (BYTE)opcode % 8);
			break;
	}
	m_pc++;
};

int Gb_core::_8bit_load(BYTE& rg, BYTE value){
	rg = value;
	return LD_8BIT_CYCLES;
};

int Gb_core::ld_r_v(BYTE& r, BYTE v){
	int cycles = 4;
	switch(v){
		case Gb_core::reg_order::REG_B: _8bit_load(r, m_registerBC.hi); break;
		case Gb_core::reg_order::REG_C: _8bit_load(r, m_registerBC.lo); break;
		case Gb_core::reg_order::REG_D: _8bit_load(r, m_registerDE.hi); break;
		case Gb_core::reg_order::REG_E: _8bit_load(r, m_registerDE.lo); break;
		case Gb_core::reg_order::REG_H: _8bit_load(r, m_registerHL.hi); break;
		case Gb_core::reg_order::REG_L: _8bit_load(r, m_registerHL.lo); break;
		case Gb_core::reg_order::REG_A: _8bit_load(r, m_registerAF.hi); break;
		case Gb_core::reg_order::REG_HL:
			_8bit_load(r, m_memory->read(m_registerHL.pair));
			cycles = 8;
			break;
		default:
			std::cout << "Unknow opcode..  " << v << std::endl;
			break;
	};
	return cycles;
};

int Gb_core::ld_addr_r(WORD addr, BYTE r){
	switch(r){
		case Gb_core::reg_order::REG_B: m_memory->write(addr, m_registerBC.hi); break;
		case Gb_core::reg_order::REG_C: m_memory->write(addr, m_registerBC.lo); break;
		case Gb_core::reg_order::REG_D: m_memory->write(addr, m_registerDE.hi); break;
		case Gb_core::reg_order::REG_E: m_memory->write(addr, m_registerDE.lo); break;
		case Gb_core::reg_order::REG_H: m_memory->write(addr, m_registerHL.hi); break;
		case Gb_core::reg_order::REG_L: m_memory->write(addr, m_registerHL.lo); break;
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
			case 0: _8bit_load(m_registerBC.hi, m_memory->read(m_pc + 1)); break;
			case 1: _8bit_load(m_registerDE.hi, m_memory->read(m_pc + 1)); break;
			case 2: _8bit_load(m_registerHL.hi, m_memory->read(m_pc + 1)); break;
			case 3:
				m_memory->write(m_registerHL.pair, m_memory->read(m_pc + 1));
				break;
		}
	}else if((opcode & 0x0f) == 0xe){
		switch((opcode & 0xf0) >> 4){
			case 0: _8bit_load(m_registerBC.lo, m_memory->read(m_pc + 1)); break;
			case 1: _8bit_load(m_registerDE.lo, m_memory->read(m_pc + 1)); break;
			case 2: _8bit_load(m_registerHL.lo, m_memory->read(m_pc + 1)); break;
			case 3: _8bit_load(m_registerAF.hi, m_memory->read(m_pc + 1)); break;
		}
	}else std::cerr << "Uknown opcode : " << (int)opcode << std::endl;
	m_pc += 2;
};

void Gb_core::_8bit_ld_xxA(){
	auto opcode = m_memory->read(m_pc);
	WORD value, addr;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::_BC_A: m_memory->write(m_registerBC.pair, m_registerAF.hi); break;
		case ld_8bit::_DE_A: m_memory->write(m_registerDE.pair, m_registerAF.hi); break;
		case ld_8bit::_HL_INC_A:
			m_memory->write(m_registerHL.pair++, m_registerAF.hi);
			break;
		case ld_8bit::_HL_DEC_A:
			m_memory->write(m_registerHL.pair--, m_registerAF.hi);
			break;
		case ld_8bit::_U16_A:
			addr = (m_memory->read(m_pc + 2) << 8)  | m_memory->read(m_pc + 1);
			m_memory->write(addr, m_registerAF.hi);
			m_pc += 2;
			break;
		default: std::cerr << "here Uknown opcode : " << (int)opcode << std::endl;
	}
	m_pc++;
};
		
void Gb_core::_8bit_ld_Axx(){
	auto opcode = m_memory->read(m_pc);
	WORD value, addr;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::A_BC_:
			value = m_memory->read(m_registerBC.pair);
			_8bit_load(m_registerAF.hi, value);
			break;
		case ld_8bit::A_DE_:
			value = m_memory->read(m_registerDE.pair);
			_8bit_load(m_registerAF.hi, value);
			break;
		case ld_8bit::A_HL_INC:
			value = m_memory->read(m_registerHL.pair++);
			_8bit_load(m_registerAF.hi, value);
			break;
		case ld_8bit::A_HL_DEC:
			value = m_memory->read(m_registerHL.pair--);
			_8bit_load(m_registerAF.hi, value);
			break;
		case ld_8bit::A_U16_:
			addr = (m_memory->read(m_pc + 2) << 8)  | m_memory->read(m_pc + 1);
			_8bit_load(m_registerAF.hi, m_memory->read(addr));
			m_pc += 2;
			break;
		default: std::cerr << "here Uknown opcode : " << (int)opcode << std::endl;
	}
	m_pc++;
};

BYTE Gb_core::r_X(reg_order r) const{
	switch(r){
		case Gb_core::reg_order::REG_B: return m_registerBC.hi;
		case Gb_core::reg_order::REG_C: return m_registerBC.lo;
		case Gb_core::reg_order::REG_D: return m_registerDE.hi;
		case Gb_core::reg_order::REG_E: return m_registerDE.lo;
		case Gb_core::reg_order::REG_H:	return m_registerHL.hi;
		case Gb_core::reg_order::REG_L: return m_registerHL.lo;
		case Gb_core::reg_order::REG_A: return m_registerAF.hi;
	};
};

void Gb_core::_16_bit_ld(){
	auto opcode = m_memory->read(m_pc);
	WORD value = (m_memory->read(m_pc + 2) << 8) | (m_memory->read(m_pc + 1));
	switch(static_cast<ld_16bit>(opcode)){
		case ld_16bit::BC_U16: m_registerBC.pair = value; break;
		case ld_16bit::DE_U16: m_registerDE.pair = value; break;
		case ld_16bit::HL_U16: m_registerHL.pair = value; break;
		case ld_16bit::SP_U16: m_sp.pair = value; break;
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

std::vector<Gb_core::alu> Gb_core::opcodes_alu() const{
	return {alu::ADD_A_B, alu::ADD_A_C, alu::ADD_A_D, alu::ADD_A_E, alu::ADD_A_H,
			alu::ADD_A_L, alu::ADD_A_HL_, alu::ADD_A_A, alu::ADC_A_B, alu::ADC_A_C,
			alu::ADC_A_D, alu::ADC_A_E, alu::ADC_A_H, alu::ADC_A_L, alu::ADC_A_HL_,
			alu::ADC_A_A, alu::SUB_A_B, alu::SUB_A_C, alu::SUB_A_D, alu::SUB_A_E,
			alu::SUB_A_H, alu::SUB_A_L, alu::SUB_A_HL_, alu::SUB_A_A, alu::SBC_A_B,
			alu::SBC_A_C, alu::SBC_A_D, alu::SBC_A_E, alu::SBC_A_H, alu::SBC_A_L,
			alu::SBC_A_HL_, alu::SBC_A_A, alu::AND_A_B, alu::AND_A_C, alu::AND_A_D,
			alu::AND_A_E, alu::AND_A_H, alu::AND_A_L, alu::AND_A_HL_, alu::AND_A_A};
};

void Gb_core::_8bit_ld_ff00(){
	auto opcode = m_memory->read(m_pc);
	WORD offset;
	switch(static_cast<ld_8bit>(opcode)){
		case ld_8bit::_FF00_U8_A:
			offset = m_memory->read(m_pc + 1);
			m_memory->write(0xff00 + offset, m_registerAF.hi);
			m_pc++;
			break;
		case ld_8bit::A_FF00_U8_:
			offset = m_memory->read(m_pc + 1);
			_8bit_load(m_registerAF.hi, m_memory->read(0xff00 + offset));
			m_pc++;
			break;
		case ld_8bit::_FF00_C_A:
			m_memory->write(0xff00 + m_registerBC.lo, m_registerAF.hi);
			break;
		case ld_8bit::A_FF00_C_:
			_8bit_load(m_registerAF.hi, m_memory->read(0xff00 + m_registerBC.lo));
			break;
		default:
			std::cerr << "Uknown opcode : " << std::hex << (int)opcode << std::endl;
	};
	m_pc++;
};

void Gb_core::push(const WORD& rr){
	m_memory->write(m_sp.pair--, (rr & 0xff00) >> 8);
	m_memory->write(m_sp.pair--, (rr & 0x00ff));
};

void Gb_core::pop(WORD& rr, bool af){
	if(m_sp.pair == SP_INIT_ADDR) return;
	if(af){
		rr = (rr & 0xff00) | (m_memory->read(++m_sp.pair));
		m_sp.pair++;
	}else{
		rr = m_memory->read(++m_sp.pair) | (m_memory->read(++m_sp.pair) << 8);
	}
};

void Gb_core::_16bit_ldsp(){
	auto opcode = m_memory->read(m_pc);
	if(col(opcode) == 0x1){
		auto& pair = m_reg16_map[row(opcode) - 12]();
		pop(pair, row(opcode) == 0xf);
	};

	if(col(opcode) == 0x5){
		auto pair = m_reg16_map[row(opcode) - 12]();
		push(pair);
	};
	m_pc++;
};

void Gb_core::set_flag(Gb_core::flag f){ m_registerAF.lo |= (0x1 << (7 - f)); };

void Gb_core::unset_flag(Gb_core::flag f){ m_registerAF.lo &= ~(0x1 << (7 - f)); };

BYTE Gb_core::get_flag(flag f){ return ((m_registerAF.lo >> (7 - f)) & 0x1); };

// ---- ----
void Gb_core::x8_alu_add(BYTE r2, bool add_carry){
	BYTE& r1 = m_registerAF.hi;
	if(add_carry){
		BYTE c_flag = ((m_registerAF.lo >> (7 - flag::CARRY)) & 0x1);
		// overflow?
		unset_flag(HALF_CARRY);
		if(((r1 & 0x0f) + (r2 + 0x0f) + c_flag) & 0x10) set_flag(flag::HALF_CARRY);
			
		unset_flag(flag::CARRY);
		if(static_cast<WORD>(r1) + static_cast<WORD>(r2) + c_flag > 0xff)
			set_flag(flag::CARRY);


		if(r1 + r2 + c_flag == 0) set_flag(flag::ZERO);
		r1 += r2 + c_flag;

	}else{
		unset_flag(flag::HALF_CARRY);
		if(((r1 & 0x0f) + (r2 + 0x0f)) & 0x10) set_flag(flag::HALF_CARRY);

		unset_flag(flag::CARRY);
		if(static_cast<WORD>(r1) + static_cast<WORD>(r2) > 0xff) set_flag(flag::CARRY);

		unset_flag(flag::ZERO);
		if(r1 + r2 == 0) set_flag(flag::ZERO);
		r1 += r2;
	};
};

void Gb_core::x8_alu_sub(BYTE r2, bool sub_carry){
	BYTE& r1 = m_registerAF.hi;
	set_flag(flag::SUBS);

	if(sub_carry){
		BYTE c_flag = ((m_registerAF.lo >> (7 - flag::CARRY)) & 0x1);
		unset_flag(flag::HALF_CARRY);
		if(static_cast<int>(r1 & 0x0f) < (static_cast<int>(r2 & 0x0f) + c_flag))
			set_flag(flag::HALF_CARRY);

		if(r1 < (r2 + c_flag))
			set_flag(flag::CARRY);

		unset_flag(flag::ZERO);
		if(r1 - (r2 + c_flag) == 0) set_flag(flag::ZERO);
	}else{
		unset_flag(flag::ZERO);
		if(r1 - r2 == 0) set_flag(flag::ZERO);

		unset_flag(flag::HALF_CARRY);
		if(static_cast<int>(r1 & 0x0f) < static_cast<int>(r2 & 0x0f))
			set_flag(flag::HALF_CARRY);
		// no borrow

		if(r1 < r2)
			set_flag(flag::CARRY);
		r1 -= r2;
	}
};

void Gb_core::x8_alu_and(BYTE r2){
	m_registerAF.hi &= r2;
	if(m_registerAF.hi == 0) set_flag(flag::ZERO);
	set_flag(flag::HALF_CARRY);
	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::SUBS);
};

void Gb_core::x8_alu_xor(BYTE r2){
	m_registerAF.hi ^= r2;
	if(m_registerAF.hi == 0) set_flag(flag::ZERO);
	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::SUBS);
};

void Gb_core::x8_alu_or(BYTE r2){
	m_registerAF.hi |= r2;
	if(m_registerAF.hi == 0) set_flag(flag::ZERO);
	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::SUBS);
};


void Gb_core::x8_alu_cp(BYTE r2){
	BYTE& a = m_registerAF.hi;

	set_flag(flag::SUBS);
	if(a == r2) set_flag(flag::ZERO);
	if((a & 0x0f) - (r2 & 0x0f) < 0) set_flag(flag::HALF_CARRY);
	if(a < r2) set_flag(flag::CARRY);
};

void Gb_core::x8_alu_inc(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r + 1);
	if(res == 0) set_flag(flag::ZERO);
	unset_flag(flag::SUBS);
	if((res & 0x0f) == 0x00) set_flag(flag::HALF_CARRY);
};

void Gb_core::x8_alu_dec(reg_order n){
	auto r = m_reg_rmap[static_cast<reg_order>(n)]();
	auto res = m_reg_wmap[n](r - 1);
	if(res == 0) set_flag(flag::ZERO);
	set_flag(flag::SUBS);
	if((res & 0x0f) == 0x0f) set_flag(flag::HALF_CARRY);
};

void Gb_core::core_alu(){

	auto opcode = m_memory->read(m_pc);
	bool carry = (opcode & 0x0f) > 7 ? true : false;


	if((alu)opcode >= alu::ADD_A_B && (alu)opcode <= alu::ADC_A_A){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_add(r2, carry);
	};

	if((alu)opcode >= alu::SUB_A_B && (alu)opcode <= alu::SBC_A_A){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_sub(r2, carry);
	};

	if((alu)opcode >= alu::AND_A_B && (alu)opcode <= alu::AND_A_A){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_and(r2);
	};

	if((alu)opcode >= alu::XOR_A_B && (alu)opcode <= alu::XOR_A_A){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_xor(r2);
	};

	if((alu)opcode >= alu::OR_START && (alu)opcode <= alu::OR_END){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_or(r2);
	};

	if((alu)opcode >= alu::CP_START && (alu)opcode <= alu::CP_END){
		auto r2 = m_reg_rmap[static_cast<reg_order>((opcode & 0x0f) % 8)]();
		x8_alu_cp(r2);
	};

	if(row(opcode) >= 0 && row(opcode) <= 3){
		int reg_num;
		if(col(opcode) == 4 || col(opcode) == 0xc){
			if(col(opcode) == 4) reg_num = row(opcode) * 2;
			else reg_num = (row(opcode) * 2) + 1;
			x8_alu_inc(static_cast<reg_order>(reg_num));
		}else if(col(opcode) == 5 || col(opcode) == 0xd){
			if(col(opcode) == 5) reg_num = row(opcode) * 2;
			else reg_num = (row(opcode) * 2) + 1;
			x8_alu_dec(static_cast<reg_order>(reg_num));
		}
	}

	m_pc++;
};


void Gb_core::x8_alu_daa(){
	BYTE& A = m_registerAF.hi;

	BYTE correction = 0;
	if(get_flag(flag::HALF_CARRY) || (!get_flag(flag::SUBS) && (A & 0xf) > 9)){
		correction |= 0x06;
	};

	if(get_flag(flag::CARRY) || (!get_flag(flag::SUBS) && (A > 0x99))){
		correction |= 0x60;
	};

	if(get_flag(flag::SUBS)){
		if(static_cast<WORD>(A - correction) & 0x100) set_flag(flag::CARRY);
		A -= correction;
	}else{
		if(static_cast<WORD>(A + correction) & 0x100) set_flag(flag::CARRY);
		A += correction;
	}
	
	if(A == 0) set_flag(flag::ZERO);
	unset_flag(flag::HALF_CARRY);

	m_pc++;
};


void Gb_core::x8_alu_cpl(){
	m_registerAF.hi = ~m_registerAF.hi;
	set_flag(flag::SUBS);
	set_flag(flag::HALF_CARRY);
};

void Gb_core::x8_alu_ccf(){
	if(get_flag(flag::CARRY)){
		unset_flag(flag::CARRY);
	}else{
		set_flag(flag::CARRY);
	}

	unset_flag(flag::HALF_CARRY);
	unset_flag(flag::SUBS);
};
