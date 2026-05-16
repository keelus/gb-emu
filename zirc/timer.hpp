#pragma once

#include <cstddef>
#include <cstdint>

namespace Zirc {
class Bus;

class Timer {
  public:
	Timer(Bus &bus) : m_bus(bus) { reset(); }

	void tick(const uint8_t tStates) {
		for(size_t i = 0; i < tStates; i++) {
			tickOne();
		}
	}

	void resetDiv() { m_clk = 0; }
	uint8_t getDiv() const { return static_cast<uint8_t>(m_clk >> 8); }

	uint8_t getTac() const { return m_tac; }
	void setTac(const uint8_t newTac) { m_tac = newTac & 0x7; }

	uint8_t getTma() const { return m_tma; }
	void setTma(const uint8_t newTma) { m_tma = newTma; }

	uint8_t getTima() const { return m_tima; }
	void setTima(const uint8_t newTima) { m_tima = newTima; }

	void reset() {
		m_tac = 0;
		m_tma = 0;
		m_tima = 0;

		m_clk = 0;
		m_prevClk = 0;
	}

  private:
	void tickOne();

	bool isTimaEnabled() const { return (m_tac & 0x4) != 0; }

	uint8_t m_tac;

	uint8_t m_tma;
	uint8_t m_tima;

	uint16_t m_clk, m_prevClk;

	Bus &m_bus;
};
}; // namespace Zirc
