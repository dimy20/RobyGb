#include "Gb_core.h"

Gb_core::Gb_core(Mem_mu * memory) : m_memory(memory) {
}

void Gb_core::dispatch_next(){
	goto *m_dispatch_table[(Is)(m_pc++)]; //jmp
};

void Gb_core::emulate_cycles(){
	m_dispatch_table[Is::LD_8BIT_B] = &&loadB;
	m_dispatch_table[Is::LD_8BIT_C] = &&loadC;
	m_dispatch_table[Is::LD_8BIT_D] = &&loadD;
	m_dispatch_table[Is::LD_8BIT_E] = &&loadE;
	m_dispatch_table[Is::LD_8BIT_H] = &&loadH;
	m_dispatch_table[Is::LD_8BIT_L] = &&loadL;
	int n = 1; // cycles;
	dispatch_next();
	for(int i = 0; i < n; i++){
		loadB: _8bit_load(m_registerBC.hi); dispatch_next();
		loadC: _8bit_load(m_registerBC.lo); dispatch_next();
		loadD: _8bit_load(m_registerDE.hi); dispatch_next(); 
		loadE: _8bit_load(m_registerDE.lo); dispatch_next();
		loadH: _8bit_load(m_registerHL.hi); dispatch_next();
		loadL: _8bit_load(m_registerHL.lo); dispatch_next();
	};
};

int Gb_core::_8bit_ld_r1r2(){
	auto opcode = (Is)m_memory->read(m_pc);
	switch(opcode){
		case Is::LD_8BIT_AA: case Is::LD_8BIT_AB: case Is::LD_8BIT_AC:
		case Is::LD_8BIT_AD: case Is::LD_8BIT_AE: case Is::LD_8BIT_AH:
		case Is::LD_8BIT_AL: case Is::LD_8BIT_AHL:
			return ld_r_v(m_registerAF.hi, (BYTE)opcode % 8);
		case Is::LD_8BIT_BB: case Is::LD_8BIT_BC: case Is::LD_8BIT_BD:
		case Is::LD_8BIT_BE: case Is::LD_8BIT_BH: case Is::LD_8BIT_BL:
		case Is::LD_8BIT_BHL:
			return ld_r_v(m_registerBC.hi, (BYTE)opcode % 8);
		case Is::LD_8BIT_CB: case Is::LD_8BIT_CC: case Is::LD_8BIT_CD:
		case Is::LD_8BIT_CE: case Is::LD_8BIT_CH: case Is::LD_8BIT_CL:
		case Is::LD_8BIT_CHL:
			return ld_r_v(m_registerBC.lo, (BYTE)opcode % 8);
		case Is::LD_8BIT_DB: case Is::LD_8BIT_DC: case Is::LD_8BIT_DD:
		case Is::LD_8BIT_DE: case Is::LD_8BIT_DH: case Is::LD_8BIT_DL:
		case Is::LD_8BIT_DHL:
			return ld_r_v(m_registerDE.hi, (BYTE)opcode % 8);
		case Is::LD_8BIT_EB: case Is::LD_8BIT_EC: case Is::LD_8BIT_ED:
		case Is::LD_8BIT_EE: case Is::LD_8BIT_EH: case Is::LD_8BIT_EL:
		case Is::LD_8BIT_EHL:
			return ld_r_v(m_registerDE.lo, (BYTE)opcode % 8);
		case Is::LD_8BIT_HB: case Is::LD_8BIT_HC: case Is::LD_8BIT_HD:
		case Is::LD_8BIT_HE: case Is::LD_8BIT_HH: case Is::LD_8BIT_HL:
		case Is::LD_8BIT_HHL:
			return ld_r_v(m_registerHL.hi, (BYTE)opcode % 8);
		case Is::LD_8BIT_LB: case Is::LD_8BIT_LC: case Is::LD_8BIT_LD:
		case Is::LD_8BIT_LE: case Is::LD_8BIT_LH: case Is::LD_8BIT_LL:
		case Is::LD_8BIT_LHL:
			return ld_r_v(m_registerHL.lo, (BYTE)opcode % 8);
		case Is::LD_8BIT_HLB: case Is::LD_8BIT_HLC: case Is::LD_8BIT_HLD:
		case Is::LD_8BIT_HLE: case Is::LD_8BIT_HLH: case Is::LD_8BIT_HLL:
			return ld_addr_r(m_registerHL.pair, (BYTE)opcode % 8);
		case Is::LD_8BIT_HLN:
			m_pc++;
			m_memory->write(m_registerHL.pair, m_memory->read(m_pc));
			return 12;
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
