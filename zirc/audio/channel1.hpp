#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace Zirc {
class Channel1 {
  public:
	Channel1() { reset(); }

	void write8(const uint16_t address, const uint8_t value) {
		switch(address) {
		case 0xFF10: m_nr10 = value & 0x7F; break;
		case 0xFF11: m_nr11 = value; break;
		case 0xFF12: m_nr12 = value; break;
		case 0xFF13: m_nr13 = value; break;
		case 0xFF14:
			m_nr14 = value & 0x7F;
			if((value & 0x80) != 0) { trigger(); }
			break;

		default: {
			std::stringstream stream;
			stream << "Audio: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	uint8_t read8(const uint16_t address) const {
		switch(address) {
		case 0xFF10: return m_nr10;
		case 0xFF11: return m_nr11 & 0xC0;
		case 0xFF12: return m_nr12;
		case 0xFF13: return 0xFF;
		case 0xFF14: return m_nr14 & 0x40;
		default: {
			std::stringstream stream;
			stream << "Audio: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	void doEventEnvelopeSweep() {
		if(m_enabledFlag && getSweepPace() != 0) {
			m_envelopeAcc++;
			if(m_envelopeAcc >= getSweepPace()) {
				m_envelopeAcc = 0;

				if((m_nr12 & 0x8) == 0 && m_volume > 0) {
					m_volume--;
				} else if((m_nr12 & 0x8) != 0 && m_volume < 15) {
					m_volume++;
				}
			}
		}
	}

	void doEventSoundLength() {
		if(m_isOn) {
			if(m_lengthTimer > 0) { m_lengthTimer--; }
			if((m_nr14 & 0x40) != 0 && m_lengthTimer == 0) { turnOff(); }
		}
	}

	void doEventFrequencySweep() {
		if(m_enabledFlag && getPace() != 0) {
			m_periodSweepAcc++;
			if(m_periodSweepAcc > getPace()) {
				m_periodSweepAcc = 0;
				uint16_t finalStep = getPeriod() / static_cast<uint16_t>(powf(2, getIndividualStep()));
				bool overflows = false;
				if(getPeriodDirection() == 0 && getPeriod() < 0x7FF) {
					uint16_t newPeriod = getPeriod() + finalStep;
					if(newPeriod > 0x7FF) {
						overflows = true;
					} else {
						setPeriod(newPeriod);
					}
				} else if(getPeriodDirection() != 0 && getPeriod() > 0) {
					uint16_t newPeriod = (getPeriod() - finalStep);
					if(newPeriod > 0x7FF) {
						overflows = true;
					} else {
						setPeriod(newPeriod);
					}
				}

				if(overflows) { turnOff(); }
			}
		}
	}

	void tick(const size_t sampleRate) {
		m_periodDivider++;
		if(m_periodDivider == 0x800) { m_periodDivider = getPeriod(); }

		if(!m_isOn) { return; }

		double freq = getFrequency();
		if(freq > static_cast<double>(sampleRate) / 2.0) { freq = static_cast<double>(sampleRate) / 2.0; }

		m_phase += 2.0 * M_PI * freq / static_cast<double>(sampleRate);
		if(m_phase >= 2.0 * M_PI) { m_phase -= 2.0 * M_PI; }
	}

	float getSample(const float amplitude) {
		uint8_t duty = (m_nr11 >> 6) & 0x3;
		double dutyPercent = 0;
		switch(duty) {
		case 0b00: dutyPercent = 12.5f; break;
		case 0b01: dutyPercent = 25.0f; break;
		case 0b10: dutyPercent = 50.0f; break;
		case 0b11: dutyPercent = 75.0f; break;
		}
		assert(dutyPercent != 0);

		double phaseThreshold = (2 * M_PI) * (dutyPercent / 100.0f);
		float value = amplitude * static_cast<float>(m_volume) / 15.0f;
		return (m_phase < phaseThreshold ? 1 : -1) * value;
	}

	bool isOn() const { return m_isOn; }

	void trigger(void) {
		turnOn();
		m_envelopeAcc = 0;
		m_volume = getInitialVolume();
		m_periodSweepAcc = 0;
		m_enabledFlag = getSweepPace() != 0 || getIndividualStep() != 0;

		if(m_lengthTimer == 0) { m_lengthTimer = 63 - (m_nr11 & 0x3F); }
	}

	bool isDacOn() const { return (m_nr12 & 0xF8) != 0; }

	void reset() {
		m_phase = 0.0;

		m_isOn = false;

		m_nr10 = 0;
		m_nr11 = 0;
		m_nr12 = 0;
		m_nr13 = 0;
		m_nr14 = 0;

		m_lengthTimer = 0;

		m_periodDivider = 0;

		m_envelopeAcc = 0;
		m_periodSweepAcc = 0;
		m_volume = 15;
		m_enabledFlag = false;
	}

  private:
	void turnOff() { m_isOn = false; }

	void turnOn() { m_isOn = true; }

	uint16_t getPeriod() const { return static_cast<uint16_t>(((m_nr14 & 0x7) << 8) | m_nr13); }
	void setPeriod(const uint16_t value) {
		m_nr13 = static_cast<uint8_t>(value & 0xFF);
		m_nr14 = static_cast<uint8_t>((m_nr14 & 0xF8) | (value >> 8));
	}

	double getFrequency() const {
		int base_freq = 131072 / (2048 - getPeriod());
		return base_freq;
	}

	uint8_t getSweepPace() const { return m_nr12 & 0x7; }
	uint8_t getPace() const { return (m_nr10 & 0x70) >> 5; }
	uint8_t getIndividualStep() const { return m_nr10 & 0x7; }
	uint8_t getPeriodDirection() const { return (m_nr10 >> 3) & 0x1; }
	uint8_t getInitialVolume() const { return (m_nr12 & 0xF0) >> 4; }

	double m_phase;

	bool m_isOn;

	uint8_t m_nr10, m_nr11, m_nr12, m_nr13, m_nr14;

	uint8_t m_lengthTimer;

	uint16_t m_periodDivider;

	int m_envelopeAcc;
	int m_periodSweepAcc;
	uint8_t m_volume;
	bool m_enabledFlag;
};
} // namespace Zirc
