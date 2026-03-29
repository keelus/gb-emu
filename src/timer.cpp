#include "timer.hpp"
#include "bus.hpp"
#include <cstdint>

void Timer::tickOne() {
	m_clk++;

	if(!isTimaEnabled()) {
		m_prevClk = m_clk;
		return;
	}

	uint16_t mask;
	uint8_t mode = m_tac & 0x3;
	bool timaIncrements = false;
	switch(mode) {
	case 0b00: mask = 1 << 9; break;
	case 0b01: mask = 1 << 3; break;
	case 0b10: mask = 1 << 5; break;
	case 0b11: mask = 1 << 7; break;
	}

	uint16_t curBit = (m_clk & mask) != 0;
	uint16_t prevBit = (m_prevClk & mask) != 0;

	if(prevBit == 1 && curBit == 0) {
		m_tima++;

		if(m_tima == 0) {
			m_tima = m_tma;
			m_bus.requestInterrupt(Bus::InterruptRequestType::Timer);
		}
	}

	m_prevClk = m_clk;
}
