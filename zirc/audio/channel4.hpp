#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

#include "lfsr.hpp"

namespace Zirc {
class Channel4 {
  public:
	Channel4() { reset(); }

	void write8(const uint16_t address, const uint8_t value) {
		switch(address) {
		case 0xFF20: m_nr41 = value; break;
		case 0xFF21: m_nr42 = value; break;
		case 0xFF22:
			m_nr43 = value;
			if(getLfsrWidth() == 0) {
				m_lfsr.setLongMode();
			} else {
				m_lfsr.setShortMode();
			}
			break;
		case 0xFF23:
			m_nr44 = value & 0x7F;
			if((value & 0x80) != 0) { trigger(); }
			break;
		default: {
			std::stringstream stream;
			stream << "Audio[CH4]: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	uint8_t read8(const uint16_t address) const {
		switch(address) {
		case 0xFF20: return 0xFF;
		case 0xFF21: return m_nr42;
		case 0xFF22: return m_nr43;
		case 0xFF23: return m_nr44 & 0x40;
		default: {
			std::stringstream stream;
			stream << "Audio[CH4]: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	void doEventEnvelopeSweep() {
		if(getSweepPace() != 0) {
			m_envelopeAcc++;
			if(m_envelopeAcc >= getSweepPace()) {
				m_envelopeAcc = 0;

				if((m_nr42 & 0x8) == 0 && m_volume > 0) {
					m_volume--;
				} else if((m_nr42 & 0x8) != 0 && m_volume < 15) {
					m_volume++;
				}
			}
		}
	}

	void doEventSoundLength() {
		if(m_isOn) {
			if(m_lengthTimer > 0) { m_lengthTimer--; }
			if((m_nr44 & 0x40) != 0 && m_lengthTimer == 0) { turnOff(); }
		}
	}

	bool isOn() const { return m_isOn; }

	void tick(const float sampleRate) {
		double ticksPerSample = static_cast<double>(getTickFrequency()) / sampleRate;

		m_tickSampleAcc += ticksPerSample;
		while(m_tickSampleAcc >= 1.0) {
			m_tickSampleAcc -= 1.0;
			m_lfsr.tick();
		}
	}

	float getSample(const float amplitude) {
		if(!m_isOn) { return 0.0f; }
		return (m_lfsr.getBit() ? -1.0f : 1.0f) * amplitude * m_volume / 15.0f;
	}

	bool isDacOn() const { return (m_nr42 & 0xF8) != 0; }

	void reset() {
		m_isOn = false;

		m_nr41 = 0;
		m_nr42 = 0;
		m_nr43 = 0;
		m_nr44 = 0;

		m_lengthTimer = 0;

		m_envelopeAcc = 0;
		m_volume = 15;

		m_lfsr.reset();

		m_tickSampleAcc = 0.0;
	}

  private:
	void trigger(void) {
		turnOn();
		m_envelopeAcc = 0;
		m_volume = getInitialVolume();
		m_lfsr.reset();
		m_tickSampleAcc = 0;

		if(m_lengthTimer == 0) { m_lengthTimer = 63 - (m_nr41 & 0x3F); }
	}

	void turnOff() { m_isOn = false; }
	void turnOn() { m_isOn = true; }

	double getTickFrequency() const {
		double clockDivider = getClockDivider();
		if(clockDivider == 0) { clockDivider = 0.5; }
		return 262144.0 / (clockDivider * powf(2, getClockShift()));
	}

	uint8_t getSweepPace() const { return m_nr42 & 0x7; }
	uint8_t getInitialVolume() const { return (m_nr42 & 0xF0) >> 4; }

	uint8_t getClockShift() const { return (m_nr43 >> 4) & 0xF; }
	uint8_t getLfsrWidth() const { return (m_nr43 >> 3) & 0x1; }
	uint8_t getClockDivider() const { return m_nr43 & 0x7; }

	bool m_isOn;

	uint8_t m_nr41, m_nr42, m_nr43, m_nr44;

	uint8_t m_lengthTimer;

	int m_envelopeAcc;
	uint8_t m_volume;

	Lfsr m_lfsr;

	double m_tickSampleAcc;
};
} // namespace Zirc
