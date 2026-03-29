#pragma once

#include "SDL_audio.h"
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

class Channel2 {
  public:
	void write8(const uint16_t address, const uint8_t value) {
		switch(address) {
		case 0xFF16: m_nr21 = value; break;
		case 0xFF17: m_nr22 = value; break;
		case 0xFF18: m_nr23 = value; break;
		case 0xFF19:
			m_nr24 = value & 0x7F;
			if((value & 0x80) != 0) { trigger(); }
			break;

		default: {
			std::stringstream stream;
			stream << "Audio[CH2]: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	uint8_t read8(const uint16_t address) const {
		switch(address) {
		case 0xFF16: return m_nr21 & 0xC0;
		case 0xFF17: return m_nr22;
		case 0xFF18: return 0xFF;
		case 0xFF19: return m_nr24 & 0x40;
		default: {
			std::stringstream stream;
			stream << "Audio[CH2]: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
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

				if((m_nr22 & 0x8) == 0 && m_volume > 0) {
					m_volume--;
				} else if((m_nr22 & 0x8) != 0 && m_volume < 15) {
					m_volume++;
				}
			}
		}
	}

	void doEventSoundLength() {
		if(m_isOn) {
			if(m_lengthTimer > 0) { m_lengthTimer--; }
			if((m_nr24 & 0x40) != 0 && m_lengthTimer == 0) { turnOff(); }
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

	void fillBuffer(float *buffer, int samples, int sampleRate, double amplitude) {
		if(!m_isOn) { return; }

		uint8_t duty = (m_nr21 >> 6) & 0x3;
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
				buffer[i] = amplitude * static_cast<float>(m_volume) / 15.0f;
			} else {
				buffer[i] = -amplitude * static_cast<float>(m_volume) / 15.0f;
			}

			double freq = getFrequency();
			if(freq > sampleRate / 2.0) { freq = sampleRate / 2.0; }

			m_phase += 2.0 * M_PI * freq / sampleRate;
			if(m_phase >= 2.0 * M_PI) { m_phase -= 2.0 * M_PI; }
		}
	}

	void trigger(void) {
		turnOn();
		m_envelopeAcc = 0;
		m_volume = getInitialVolume();

		if(m_lengthTimer == 0) { m_lengthTimer = 63 - (m_nr21 & 0x3F); }
	}

	bool isDacOn() const { return (m_nr22 & 0xF8) != 0; }

  private:
	void turnOff() {
		m_isOn = false;
		SDL_PauseAudio(1);
	}

	void turnOn() {
		m_isOn = true;
		SDL_PauseAudio(0);
	}

	uint16_t getPeriod() const { return (static_cast<uint16_t>(m_nr24 & 0x7) << 8) | static_cast<uint16_t>(m_nr23); }
	void setPeriod(const uint16_t value) {
		m_nr23 = value & 0xFF;
		m_nr24 = (m_nr24 & 0xF8) | (value >> 8);
	}

	double getFrequency() const {
		int base_freq = 131072 / (2048 - getPeriod());
		return base_freq;
	}

	uint8_t getSweepPace() const { return m_nr22 & 0x7; }
	uint8_t getInitialVolume() const { return (m_nr22 & 0xF0) >> 4; }

	double m_phase;

	bool m_isOn = false;

	uint8_t m_nr21 = 0;
	uint8_t m_nr22 = 0;
	uint8_t m_nr23 = 0;
	uint8_t m_nr24 = 0;

	uint8_t m_lengthTimer = 0;

	uint16_t m_periodDivider = 0;

	int m_envelopeAcc = 0;
	uint8_t m_volume = 15;
};
