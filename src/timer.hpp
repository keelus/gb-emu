#pragma once

#include <cstddef>
#include <cstdint>

class Bus;

class Timer {
  public:
	Timer(Bus &bus) : m_bus(bus) {
		m_div = 0xAB;
		m_tac = 0;
		m_tma = 0;
		m_tima = 0;
		m_timaAccTStates = 0;

		m_tStateAcc = 0;
	}

	void tick(const uint8_t tStates) {
		for(size_t i = 0; i < tStates; i++) {
			tickOne();
		}
	}

	void resetDiv() { m_div = 0; }
	uint8_t getDiv() const { return m_div; }

	uint8_t getTac() const { return m_tac; }
	void setTac(const uint8_t newTac) { m_tac = newTac & 0x7; }

	uint8_t getTma() const { return m_tma; }
	void setTma(const uint8_t newTma) { m_tma = newTma; }

	uint8_t getTima() const { return m_tima; }
	void setTima(const uint8_t newTima) { m_tima = newTima; }

  private:
	void tickOne();

	bool isTimaEnabled() const { return (m_tac & 0x4) != 0; }

	uint16_t timaTStatesPerIncrement() const {
		uint8_t mode = m_tac & 0x3;
		switch(mode) {
		case 0b00: return 1024;
		case 0b01: return 16;
		case 0b10: return 64;
		case 0b11: return 256;
		}

		return 0;
	}

	uint8_t m_div;

	uint8_t m_tac;

	uint8_t m_tma;

	uint16_t m_timaAccTStates;
	uint8_t m_tima;

	size_t m_tStateAcc;

	Bus &m_bus;
};
