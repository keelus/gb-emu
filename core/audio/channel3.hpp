#pragma once

#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>

class Channel3 {
  public:
	void write8(const uint16_t address, const uint8_t value) {
		switch(address) {
		case 0xFF1A: m_nr30 = value & 0x80; break;
		case 0xFF1B: m_nr31 = value; break;
		case 0xFF1C: m_nr32 = value & 0x60; break;
		case 0xFF1D: m_periodDivider = (m_periodDivider & 0x700) | static_cast<uint16_t>(value); break;
		case 0xFF1E:
			if(value & 0x80) { trigger(); }
			m_periodDivider = (m_periodDivider & 0xFF) | ((static_cast<uint16_t>(value) << 8) & 0x700);
			m_lengthEnabled = (value & 0x40) != 0;
			break;
		default: {
			if(address >= 0xFF30 && address <= 0xFF3F) {
				m_wave[address - 0xFF30] = value;
				return;
			}

			std::stringstream stream;
			stream << "Audio[CH2]: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	uint8_t read8(const uint16_t address) const {
		switch(address) {
		case 0xFF1A: return m_nr30;
		case 0xFF1C: return m_nr32;
		case 0xFF1E: return static_cast<uint8_t>(m_lengthEnabled) << 6; break;
		default: {
			if(address >= 0xFF30 && address <= 0xFF3F) { return m_wave[address - 0xFF30]; }

			std::stringstream stream;
			stream << "Audio[CH2]: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	void doEventSoundLength() {
		if(m_isOn && m_lengthEnabled) {
			if(m_lengthTimer > 0) { m_lengthTimer--; }
			if(m_lengthTimer == 0) { turnOff(); }
		}
	}

	bool isOn() const { return m_isOn; }

	void tick(float sampleRate) {
		double freq = getFrequency();
		if(freq > sampleRate / 2.0) freq = sampleRate / 2.0;

		double step = (freq * 32.0) / sampleRate;

		m_phase += step;
		while(m_phase >= 1.0) {
			m_phase -= 1.0;
			m_waveIndex = (m_waveIndex + 1) & 31;
		}
	}

	float getSample(float amplitude) {
		float volume;
		switch((m_nr32 >> 5) & 0x03) {
		case 0: volume = 0.0f; break;
		case 1: volume = 1.0f; break;
		case 2: volume = 0.5f; break;
		case 3: volume = 0.25f; break;
		}

		uint8_t value = m_wave[m_waveIndex / 2];
		value = ((m_waveIndex % 2 == 0) ? (value >> 4) : value) & 0xF;

		float normalizedValue = ((static_cast<float>(value) * volume) / 15.0f) * 2.0f - 1.0f;
		return normalizedValue * amplitude;
	}

	void trigger(void) {
		turnOn();
		m_waveIndex = 0;

		if(m_lengthTimer == 0) { m_lengthTimer = 256; }
	}

	bool isDacOn() const { return (m_nr30 & 0x80) != 0; }

  private:
	void turnOff() { m_isOn = false; }

	void turnOn() { m_isOn = true; }

	double getFrequency() const { return 65536.0 / (2048 - m_periodDivider); }

	double m_phase = 0;

	bool m_isOn = false;

	uint8_t m_nr30 = 0;
	uint8_t m_nr31 = 0;
	uint8_t m_nr32 = 0;

	bool m_lengthEnabled = false;

	uint16_t m_periodDivider = 0;

	uint8_t m_wave[32] = {0};
	uint8_t m_waveIndex = 0;

	uint16_t m_lengthTimer = 0;
};
