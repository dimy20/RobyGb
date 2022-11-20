#pragma once

#define CLOCK_SPEED 4194304
#include "Gb_types.h"
#include "Gb_interrupts.h"

class Mem_mu;
class Gb_timer{
	friend Mem_mu;
	public:
		Gb_timer() = default;
		void init(Gb_interrupts * intrs);
		void cycle(int cycles);
		void set_TMA(BYTE value);
		void set_TIMA(BYTE value);
		void set_TAC(BYTE value);
		void set_DIV(void);

		constexpr BYTE TMA () const { return m_TMA; };
		constexpr BYTE DIV() const { return m_counter >> 8; };
		constexpr BYTE TIMA() const { return m_TIMA; };
		constexpr BYTE TAC() const { return m_TAC; };

	private:
		void tick(int delta);

		constexpr BYTE tima_enabled() const { return m_TAC & 4; };
		constexpr BYTE tima_speed() const { return m_TAC & 3; };
		void check_overflow(unsigned short tima);
	private:
		Gb_interrupts * m_intrs;

		BYTE m_TIMA = 0; // main timer
		BYTE m_TAC = 0; // control
		BYTE m_TMA = 0; // modulo

		int m_prev_cycles = 0;

		const unsigned int m_speeds[4] = {1024, 16, 64, 256}; // TIMA clock speeds.
		// Timer internal counter; MSB is the div register;
		unsigned short m_counter = 0xabcc;
		// variables to handle overflow delayed by 4 clocks.
		int m_tima_delay = 0;
		bool m_overflow = false;

		int m_tma_overwrite_window = 0;
		bool m_div_write = false;
};

