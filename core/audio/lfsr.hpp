#pragma once

#include <cstdint>

class Lfsr {
  public:
	void tick() {
		uint16_t result = (m_currentState & 0x1) == ((m_currentState >> 1) & 0x1);

		m_currentState &= ~(1 << 15);
		m_currentState |= (result << 15);

		if(m_isShortMode) {
			m_currentState &= ~(1 << 7);
			m_currentState |= (result << 7);
		}

		m_currentState >>= 1;
	}

	void reset() { m_currentState = 0x7FF; }

	uint8_t getBit() const { return m_currentState & 0x1; }

	void setShortMode() { m_isShortMode = true; }
	void setLongMode() { m_isShortMode = false; }

  private:
	bool m_isShortMode;
	uint16_t m_currentState;
};
