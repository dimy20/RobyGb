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

int Gb_core::_8bit_load(BYTE& rg){ 
	rg = m_memory->read(m_pc); 
	return LD_8BIT_CYCLES;
};
