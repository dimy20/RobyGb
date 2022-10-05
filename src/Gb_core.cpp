#include "Gb_core.h"
#include <variant>
Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {
}

void Gb_core::init(){
	// initialize all 8 bit load instructions in the range [0x40, 0x7f];
	for(int i = 0x40; i <= 0x7f; i++){
		if(i == 0x76) continue; // TODO: handle halt

		int cycles = 4;
		if((i >= 0x70 && i <= 0x77) || (i % 8 == 6)) cycles = 8;
		
		ld_8bit op = static_cast<ld_8bit>(i);
		auto ptr = std::make_shared<Gb_instruction>(op, &Gb_core::_8bit_ld_r1r2, cycles);
		m_8bit_load_table[op] = ptr;
	};

	// instruction load inmediate u8 data for registers B, D, H, (HL)
	for(int i = 0x06; i <= 0x36; i += 0x10){
		int cycles = i != 0x036 ? 8 : 12;
		ld_8bit op = static_cast<ld_8bit>(i);
		auto ptr = std::make_shared<Gb_instruction>(op, &Gb_core::_8bit_ldu8, cycles);
		m_8bit_ldu8_table[op] = ptr;
	}

	// instruction load inmediate u8 data for registers C, E, L, A
	for(int i = 0x0e; i <= 0x3e; i += 0x10){
		ld_8bit op = static_cast<ld_8bit>(i);
		auto ptr = std::make_shared<Gb_instruction>(op, &Gb_core::_8bit_ldu8, 8);
		m_8bit_ldu8_table[op] = ptr;
	}

	// jmp calls
	auto ptr = std::make_shared<Gb_instruction>(i_control::JMP_NN, &Gb_core::jmp_nn, 16);
	m_jmp_table[i_control::JMP_NN] = ptr;

};

// real cycles not taken into account for now.
void Gb_core::emulate_cycles(int n){
	for(int i = 0; i < n; i++){
		auto opcode = m_memory->read(m_pc);
		//auto opcode = m_memory->read(m_pc);
		if(m_8bit_load_table.find(static_cast<ld_8bit>(opcode)) != m_8bit_load_table.end()){
			auto op = m_8bit_load_table[static_cast<ld_8bit>(opcode)];
			(this->*op->fn)(); // run instruction handler
		}else if(m_jmp_table.find(static_cast<i_control>(opcode)) != m_jmp_table.end()){
			auto op = m_jmp_table[static_cast<i_control>(opcode)];
			(this->*op->fn)(); // run instruction handler
		}else if(m_8bit_ldu8_table.find(static_cast<ld_8bit>(opcode)) != m_8bit_ldu8_table.end()){
			auto op = m_8bit_ldu8_table[static_cast<ld_8bit>(opcode)];
			(this->*op->fn)(); // run instruction handler
		}else{
			std::cerr << "Uknown opcode : " << (int)opcode << std::endl;
		}
	}
};

void Gb_core::_8bit_ld_r1r2(){
	std::cout << "8-bit load instruction" << std::endl;
	//auto opcode = (Is)m_memory->read(m_pc);
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
	std::cout << "Jumping to  : " << nn << std::endl;
};

void Gb_core::_8bit_ldu8(){
	auto opcode = m_memory->read(m_pc);

	std::cout << "Executing ld r, u8 instruction..." << std::endl;
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
