#pragma once

#include "channel1.hpp"
#include "common.hpp"
#include <SDL_audio.h>
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <iostream>

#define SAMPLE_RATE 44100
#define AMPLITUDE 0.2f
#define SAMPLES 1024

#include "SDL_audio.h"
class Apu {
  public:
	Apu() {
		std::cout << "Audio initialized." << std::endl;

		SDL_AudioSpec spec = {0};
		spec.freq = SAMPLE_RATE;
		spec.format = AUDIO_F32SYS;
		spec.channels = 1;
		spec.samples = SAMPLES;
		spec.callback = audio_callback;
		spec.userdata = this;

		SDL_OpenAudio(&spec, NULL);
	}

	void write8(const uint16_t address, const uint8_t value) {
		std::cout << "Write address 0x" << std::hex << std::setw(4) << std::setfill('0') << address << std::endl;
		if(IN_RANGE(address, 0xFF10, 0xFF14)) { return m_channel1.write8(address, value); }

		switch(address) {
		case 0xFF26: m_audioEnabled = (value & 0x80) != 0; break;
		default: {
			std::stringstream stream;
			stream << "Audio: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}

	uint8_t read8(const uint16_t address) const {
		if(IN_RANGE(address, 0xFF10, 0xFF14)) { return m_channel1.read8(address); }

		switch(address) {
		case 0xFF26: return (m_audioEnabled ? (1 << 7) : 0) | (m_channel1.isOn() ? 1 : 0);
		default: {
			std::stringstream stream;
			stream << "Audio: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
		}
	}


	void increaseDiv() {
		m_divApu++;

		if(m_divApu % 8 == 0) { m_channel1.doEventEnvelopeSweep(); }
		if(m_divApu % 2 == 0) { m_channel1.doEventSoundLength(); }
		if(m_divApu % 4 == 0) { m_channel1.doEventFrequencySweep(); }
	}

	void tick(const int tStates) {
		assert(tStates % 4 == 0);
		m_channel1.tick(tStates);
	}

  private:
	static void audio_callback(void *userData, Uint8 *stream, int len) {
		Apu *audio = static_cast<Apu *>(userData);

		int samples = len / sizeof(float);
		float *buffer = (float *)stream;

		static float channel1Buffer[SAMPLES];
		audio->m_channel1.fillBuffer(channel1Buffer, samples);

		for(int i = 0; i < samples; i++) {
			buffer[i] = channel1Buffer[i];
		}
	}

	void triggerChannel1(void) { m_channel1.trigger(); }

	bool m_audioEnabled;
	uint8_t m_divApu = 0;

	Channel1 m_channel1;
};
