#include "timer.hpp"
#include "bus.hpp"

void Timer::tickOne() {
	m_div++;
	if(!isTimaEnabled()) { return; }

	m_timaAccTStates++;

	if(m_timaAccTStates >= timaTStatesPerIncrement()) {
		m_timaAccTStates %= timaTStatesPerIncrement();

		if(++m_tima == 0) {
			m_tima = m_tma;
			m_bus.requestInterrupt(Bus::InterruptRequestType::Timer);
		}
	}
}
