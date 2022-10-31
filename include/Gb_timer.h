#pragma once

#define CLOCK_SPEED 4194304

class Mem_mu;
class Gb_timer{
	friend Mem_mu;
	public:
		Gb_timer() = default;
		void init(Mem_mu * memory);
		void update(int cycles);
	private:
		void send_interrupt(int i);
		bool enabled(); // timer enbaled?
		void update_divider(int cycles); // updates devider register
		void update_frequency(); // updates timer frequency acording to TAC
	private:
		int m_timer_counter;
		int m_divider_counter;
		int m_frequency;
		Mem_mu * m_memory;
};
