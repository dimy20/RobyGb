#include "Gb_timer.h"
#include "Memory.h"

void Gb_timer::init(Gb_interrupts * intrs){ m_intrs = intrs; };

void Gb_timer::tick(int delta){
	if(m_tma_overwrite_window)
		m_tma_overwrite_window -= delta;

	/* Ugly fix : An increment happens when a write is done to DIV register, this is a bug on the gb
	 * that needs to be emulated, the previous implementation handled the increment sucessfully for speeds 
	 * 0, 2 and 3, but failed for speed 1 (the fastest, 16 clocks), this is a fix for that edge case.*/
	if(delta - 8 >= 0 && tima_enabled() && m_speeds[tima_speed()] == 16 && m_counter & 8 && m_div_write){
		m_counter = 0;
		m_TIMA++;
		check_overflow(m_TIMA);
		m_div_write = false;
	}

	while(delta--){
		// handle delayed overflow.
		if(m_overflow){
			m_tima_delay--;
			// cycle [A]
			if(m_tima_delay <= 0){
				if(m_TIMA != 0){ // writing to TIMA during this cycle prevents IF request.
					m_overflow = false;
					m_tma_overwrite_window = 0;
					m_tima_delay = 0;
				}else{
					m_intrs->request(2);
					m_TIMA = m_TMA; // set back to modulo
					m_overflow = false;
					m_tma_overwrite_window = 4; // 4 clock window where TMA value can be chaged.
				}
			}
		}

		/*
		 * Counter increments once every cycle (4 clocks)
		 * 1M elapsed -> += 4 ticks
		 * After 256 T-clocks, the internal counter wil overflow and DIV register will increment.
		 * DIV register = MSB of the internal counter.
		 */

		auto prev_counter = m_counter;
		m_counter++;

		/*
		 * TIMA is increased by selecting either bit 3, 5, 7 or 9 from the internal
		 * counter, which bit is selected depends on the selected speed for the clock
		 * located at TAC bits 1-0.
		 * Bit 3 -> speed 1
		 * Bit 5 -> speed 2
		 * Bit 7 -> speed 3
		 * Bit 9 -> speed 0
		 * It then ands this bit with the TAC enable bit.
		 * As soon as this bit goes from 1 to 0 TIMA is incremented.
		 */

		int prev_state = prev_counter & (m_speeds[tima_speed()] >> 1);
		int curr_state = m_counter & (m_speeds[tima_speed()] >> 1);

		// bit goes from 1 -> 0
		if(tima_enabled() && (prev_state != 0 && curr_state == 0)){
			m_TIMA++;
			check_overflow(m_TIMA);
			// if it overflows we have reached cycle A
			break;
		}

	};
};


void Gb_timer::set_TAC(BYTE value){
	bool glitch = false;
	bool old_enabled = tima_enabled();
	bool old_speed = tima_speed();
	bool new_enabled = value & 4;
	bool new_speed = value & 3;

	/* Changing the value in the TAC register can have some unexpected increments in TIMA.*/
	if(old_enabled == 0){
		glitch = false;
	}else{
		/* Glitch 1
		 * If TAC's enabled bit goes from high to low and if the old corresponding speed bit
		 * is set in the internal counter, then TIMA will increment. */
		if(new_enabled == 0){ // enabled bit : 1 -> 0
			glitch = (m_counter & (m_speeds[old_speed] >> 1)) != 0;
		}else{
			/* Glitch 2
			 * When changing TAC value, if the old speed bit was 0 and the new one is 1,
			 * and the new TAC value's enabled bit is 1, TIMA will increment. */
			glitch = ((m_counter & (m_speeds[old_speed] >> 1)) != 0) && ((m_counter & (m_speeds[new_speed] >> 1)) == 0);
		}
	};

	if(glitch)
		check_overflow(++m_TIMA);
	m_TAC = value;
};

void Gb_timer::set_DIV(){
	if(m_speeds[tima_speed() == 16]){ // speed 1 fix
		m_div_write = true;
	}
	/* Glitch 3
	 * Any attempt to write to the DIV register resets it back to 0, resetting the
	 * internal counter as well since DIV is just the MSB of the internal counter.
	 * This causes the current clock speed bit (3, 5, 7, or 9) to go from 1 to 0,
	 * so it triggers a TIMA increment.
	 * */
	int old_bit = m_counter & (m_speeds[tima_speed()] >> 1);
	m_counter = 0; // write to div resets it to 0.

	if(old_bit){ // 1 -> 0
		check_overflow(++m_TIMA);
	}
};

void Gb_timer::set_TMA(BYTE value){
	// does this write happen inmediatly after the 4 clock delay after TIMA overflowed?
	if(m_tma_overwrite_window){
		m_TIMA = value;
		m_tma_overwrite_window = 0;
	}
	m_TMA = value;
};

void Gb_timer::set_TIMA(BYTE value){
	if(m_tma_overwrite_window){
		return;
	}
	m_TIMA = value;
};

void Gb_timer::check_overflow(unsigned short tima){
	if((tima & 0xff) == 0){ // overflow
		// overflow handling is delayed by 4 clocks.
		m_TIMA = 0; // during delay tima holds the value 0x00.
		m_tima_delay = 4;
		m_overflow = true;
	}
};

void Gb_timer::cycle(int cycles){
	if(cycles < m_prev_cycles) // we've entered next frame.
		m_prev_cycles = 0;

	int delta = cycles - m_prev_cycles; // elapsed clocks since last execution
	m_prev_cycles = cycles;

	tick(delta);
};
