#pragma once
#include <cassert>
class Gb_interrupts{
	public:
		Gb_interrupts() = default;
		constexpr void request(int i); // requests an interrupt

		constexpr BYTE IE() const { return m_IE; };
		constexpr BYTE IF() const { return m_IF; };

		constexpr void set_IF(BYTE value){ m_IF = value; };
		constexpr void set_IE(BYTE value){ m_IE = value; };

		constexpr bool enabled(int i){ return ((m_IE >> i) & 1); };
		constexpr bool pending(int i){ return ((m_IF >> i) & 1); };
		constexpr void ack(int i) { m_IF &= ~(1 << i); };

		constexpr WORD src(int i) const;

		// no interrupts enabled nor pending.
		constexpr bool empty () const{ return !(m_IE & m_IF & 0x1f); };
	private:
		BYTE m_IE = 0;// Interrupt enable
		BYTE m_IF = 0;// Interrupt flag (requests)
		const WORD m_srcs[5]= {0x40, 0x48, 0x50, 0x58, 0x60}; // interrupts srcs.
};

constexpr void Gb_interrupts::request(int i){
	assert(i >= 0 && i <= 4);
	m_IF |= 1 << i; 
};
constexpr WORD Gb_interrupts::src(int i) const {
	assert(i >= 0 && i <= 4);
	return m_srcs[i]; 
};

