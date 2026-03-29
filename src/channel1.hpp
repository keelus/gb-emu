#pragma once

#include "SDL_audio.h"
#include <bitset>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

#define SAMPLE_RATE 44100
#define AMPLITUDE 0.2f

class Channel1 {
  public:
	void write8(const uint16_t address, const uint8_t value) {
		switch(address) {
		case 0xFF10: m_nr10 = value & 0x7F; break;
		case 0xFF11:
			m_nr11 = value;
			trigger();
			break;
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
		if(getSweepPace() > 0) {
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
		if(getPace() > 0) {
			m_periodSweepAcc++;
			if(m_periodSweepAcc > getPace()) {
				m_periodSweepAcc = 0;
				uint8_t finalStep = getPeriod() / powf(2, m_nr10 & 0x7);
				bool overflows = false;
				int limit = 2047;
				if((m_nr10 & 0x8) == 0 && getPeriod() < limit) {
					int newPeriod = getPeriod() + finalStep;
					if(newPeriod > limit) {
						overflows = true;
					} else {
						setPeriod(newPeriod);
					}
				} else if((m_nr10 & 0x8) != 0 && getPeriod() > 0) {
					int newPeriod = (getPeriod() - finalStep);
					if(newPeriod > limit) {
						overflows = true;
					} else {
						setPeriod(newPeriod);
					}
				}

				if(overflows) { turnOff(); }
			}
		}
	}

	void tick(const int tStates) {
		assert(tStates % 4 == 0);
		for(size_t i = 0; i < tStates; i += 4) {
			m_periodDivider++;
			if(m_periodDivider == 0x800) { m_periodDivider = getPeriod(); }
		}
	}

	bool isOn() const { return m_isOn; }

	void fillBuffer(float *buffer, int samples) {
		if(!m_isOn) { return; }

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
		for(int i = 0; i < samples; i++) {
			if(m_phase < phaseThreshold) {
				buffer[i] = AMPLITUDE * static_cast<float>(m_volume) / 15.0f;
			} else {
				buffer[i] = -AMPLITUDE * static_cast<float>(m_volume) / 15.0f;
			}

			double freq = getFrequency();
			if(freq > SAMPLE_RATE / 2.0) { freq = SAMPLE_RATE / 2.0; }

			m_phase += 2.0 * M_PI * freq / SAMPLE_RATE;
			if(m_phase >= 2.0 * M_PI) { m_phase -= 2.0 * M_PI; }
		}
	}

	void trigger(void) {
		turnOn();
		m_volume = getInitialVolume();

		if(m_lengthTimer == 0) {
			std::cout << "Starting ch1 counter: 0b" << std::bitset<6>(m_nr11 & 0x3F) << std::endl;
			m_lengthTimer = 63 - (m_nr11 & 0x3F); // TODO: Investigate this
		}
	}

	bool isDacOn() const { return (m_nr12 & 0xF8) != 0; }

  private:
	void turnOff() {
		m_isOn = false;
		SDL_PauseAudio(1);
	}

	void turnOn() {
		m_isOn = true;
		SDL_PauseAudio(0);
	}


	uint16_t getPeriod() const { return (static_cast<uint16_t>(m_nr14 & 0x7) << 8) | static_cast<uint16_t>(m_nr13); }
	void setPeriod(const uint16_t value) {
		m_nr13 = value & 0xFF;
		m_nr14 = (m_nr14 & 0xF8) | (value >> 8);
	}

	double getFrequency() const {
		int base_freq = 131072 / (2048 - getPeriod());
		return base_freq;
	}

	uint8_t getSweepPace() const { return m_nr12 & 0x7; }
	uint8_t getPace() const { return (m_nr10 & 0x70) >> 5; }
	uint8_t getInitialVolume() const { return (m_nr12 & 0xF0) >> 4; }

	double m_phase;

	bool m_isOn = false;

	uint8_t m_nr10 = 0;
	uint8_t m_nr11 = 0;
	uint8_t m_nr12 = 0;
	uint8_t m_nr13 = 0;
	uint8_t m_nr14 = 0;

	uint8_t m_lengthTimer = 0;

	uint16_t m_periodDivider = 0;

	int m_envelopeAcc = 0;
	int m_periodSweepAcc = 0;
	uint8_t m_volume = 15;
};
