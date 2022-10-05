#include "Gb_core.h"

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {
}

void Gb_core::init(){
	// initialize all 8 bit load instructions in the range [0x40, 0x7f];
	for(int i = 0x40; i <= 0x7f; i++){
		if(i == 0x76) continue; // TODO: handle halt

		int cycles = 4;
		if((i >= 0x70 && i <= 0x77) || (i % 8 == 6)) cycles = 8;
		
		auto ptr = std::make_shared<opcode_t>((Is)i, &Gb_core::_8bit_ld_r1r2, cycles);
		m_8bit_load_table[(Is)(i)] = ptr;
	};

	// instruction load inmediate u8 data for registers B, D, H, (HL)
	for(int i = 0x06; i <= 0x36; i += 0x10){
		int cycles = i != 0x036 ? 8 : 12;
		auto ptr = std::make_shared<opcode_t>((Is)i, &Gb_core::_8bit_ldu8, cycles);
		m_8bit_ldu8_table[static_cast<Is>(i)] = ptr;
	}

	// instruction load inmediate u8 data for registers C, E, L, A
	for(int i = 0x0e; i <= 0x3e; i += 0x10){
		auto ptr = std::make_shared<opcode_t>((Is)i, &Gb_core::_8bit_ldu8, 8);
		m_8bit_ldu8_table[static_cast<Is>(i)] = ptr;
	}

	// jmp calls
	auto ptr = std::make_shared<opcode_t>(Is::JMP_NN, &Gb_core::jmp_nn, 16);
	m_jmp_table[Is::JMP_NN] = ptr;

};

// real cycles not taken into account for now.
void Gb_core::emulate_cycles(int n){
	for(int i = 0; i < n; i++){
		auto opcode = static_cast<Is>(m_memory->read(m_pc));
		//auto opcode = m_memory->read(m_pc);
		if(m_8bit_load_table.find(opcode) != m_8bit_load_table.end()){
			auto op = m_8bit_load_table[opcode];
			(this->*op->fn)(); // run instruction handler
		}else if(m_jmp_table.find(opcode) != m_jmp_table.end()){
			auto op = m_jmp_table[opcode];
			(this->*op->fn)(); // run instruction handler
		}else if(m_8bit_ldu8_table.find(opcode) != m_8bit_ldu8_table.end()){
			auto op = m_8bit_ldu8_table[opcode];
			(this->*op->fn)(); // run instruction handler
		}else{
			std::cerr << "Uknown opcode : " << (int)opcode << std::endl;
		}
	}
};

void Gb_core::_8bit_ld_r1r2(){
	std::cout << "8-bit load instruction" << std::endl;
	auto opcode = (Is)m_memory->read(m_pc);
	switch(opcode){
		case Is::LD_8BIT_AA: case Is::LD_8BIT_AB: case Is::LD_8BIT_AC:
		case Is::LD_8BIT_AD: case Is::LD_8BIT_AE: case Is::LD_8BIT_AH:
		case Is::LD_8BIT_AL: case Is::LD_8BIT_AHL:
			ld_r_v(m_registerAF.hi, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_BB: case Is::LD_8BIT_BC: case Is::LD_8BIT_BD:
		case Is::LD_8BIT_BE: case Is::LD_8BIT_BH: case Is::LD_8BIT_BL:
		case Is::LD_8BIT_BHL: case Is::LD_8BIT_BA:
			ld_r_v(m_registerBC.hi, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_CB: case Is::LD_8BIT_CC: case Is::LD_8BIT_CD:
		case Is::LD_8BIT_CE: case Is::LD_8BIT_CH: case Is::LD_8BIT_CL:
		case Is::LD_8BIT_CHL: case Is::LD_8BIT_CA:
			ld_r_v(m_registerBC.lo, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_DB: case Is::LD_8BIT_DC: case Is::LD_8BIT_DD:
		case Is::LD_8BIT_DE: case Is::LD_8BIT_DH: case Is::LD_8BIT_DL:
		case Is::LD_8BIT_DHL: case Is::LD_8BIT_DA:
			ld_r_v(m_registerDE.hi, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_EB: case Is::LD_8BIT_EC: case Is::LD_8BIT_ED:
		case Is::LD_8BIT_EE: case Is::LD_8BIT_EH: case Is::LD_8BIT_EL:
		case Is::LD_8BIT_EHL: case Is::LD_8BIT_EA:
			ld_r_v(m_registerDE.lo, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_HB: case Is::LD_8BIT_HC: case Is::LD_8BIT_HD:
		case Is::LD_8BIT_HE: case Is::LD_8BIT_HH: case Is::LD_8BIT_HL:
		case Is::LD_8BIT_HHL: case Is::LD_8BIT_HA:
			ld_r_v(m_registerHL.hi, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_LB: case Is::LD_8BIT_LC: case Is::LD_8BIT_LD:
		case Is::LD_8BIT_LE: case Is::LD_8BIT_LH: case Is::LD_8BIT_LL:
		case Is::LD_8BIT_LHL: case Is::LD_8BIT_LA:
			ld_r_v(m_registerHL.lo, (BYTE)opcode % 8);
			break;
		case Is::LD_8BIT_HLB: case Is::LD_8BIT_HLC: case Is::LD_8BIT_HLD:
		case Is::LD_8BIT_HLE: case Is::LD_8BIT_HLH: case Is::LD_8BIT_HLL:
		case Is::LD_8BIT_HLA:
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
		switch(opcode & 0xf0){
			case 0: _8bit_load(m_registerBC.hi, m_memory->read(m_pc + 1)); break;
			case 1: _8bit_load(m_registerDE.hi, m_memory->read(m_pc + 1)); break;
			case 2: _8bit_load(m_registerHL.hi, m_memory->read(m_pc + 1)); break;
			case 3:
				m_memory->write(m_registerHL.pair, m_memory->read(m_pc + 1));
				break;
		}
	}else if((opcode & 0x0f) == 0xe){
		switch(opcode & 0xf0){
			case 0: _8bit_load(m_registerBC.lo, m_memory->read(m_pc + 1)); break;
			case 1: _8bit_load(m_registerDE.lo, m_memory->read(m_pc + 1)); break;
			case 2: _8bit_load(m_registerHL.lo, m_memory->read(m_pc + 1)); break;
			case 3: _8bit_load(m_registerAF.hi, m_memory->read(m_pc + 1)); break;
		}
	}else std::cerr << "Uknown opcode : " << (int)opcode << std::endl;
	m_pc += 2;
};
