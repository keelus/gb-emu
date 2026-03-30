#pragma once

#include "channel1.hpp"
#include "channel2.hpp"
#include "channel4.hpp"
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
		if(IN_RANGE(address, 0xFF10, 0xFF14)) { return m_channel1.write8(address, value); }
		if(IN_RANGE(address, 0xFF16, 0xFF19)) { return m_channel2.write8(address, value); }
		if(IN_RANGE(address, 0xFF20, 0xFF23)) { return m_channel4.write8(address, value); }

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
		if(IN_RANGE(address, 0xFF16, 0xFF19)) { return m_channel2.read8(address); }
		if(IN_RANGE(address, 0xFF20, 0xFF23)) { return m_channel4.read8(address); }

		switch(address) {
		case 0xFF26:
			return (m_audioEnabled ? (1 << 7) : 0) | (m_channel4.isOn() ? (1 << 3) : 0) |
				   (m_channel2.isOn() ? (1 << 1) : 0) | (m_channel1.isOn() ? 1 : 0);
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

		if(m_divApu % 8 == 0) {
			m_channel1.doEventEnvelopeSweep();
			m_channel2.doEventEnvelopeSweep();
			m_channel4.doEventEnvelopeSweep();
		}
		if(m_divApu % 2 == 0) {
			m_channel1.doEventSoundLength();
			m_channel2.doEventSoundLength();
			m_channel4.doEventSoundLength();
		}
		if(m_divApu % 4 == 0) { m_channel1.doEventFrequencySweep(); }
	}

	void tick(const int tStates) {
		assert(tStates % 4 == 0);
		m_channel1.tick(tStates);
		m_channel2.tick(tStates);

		static bool prevAllChannelsOff = false;
		bool allChannelsOff = !(m_channel1.isOn() | m_channel2.isOn() | m_channel4.isOn());
		if(allChannelsOff != prevAllChannelsOff) { SDL_PauseAudio(allChannelsOff); }
		prevAllChannelsOff = allChannelsOff;
	}

  private:
	static void audio_callback(void *userData, Uint8 *stream, int len) {
		Apu *audio = static_cast<Apu *>(userData);

		int samples = len / sizeof(float);
		float *buffer = (float *)stream;

		memset(buffer, 0, len);

		audio->m_channel1.fillBuffer(buffer, samples, SAMPLE_RATE, AMPLITUDE);
		audio->m_channel2.fillBuffer(buffer, samples, SAMPLE_RATE, AMPLITUDE);
		audio->m_channel4.fillBuffer(buffer, samples, SAMPLE_RATE, AMPLITUDE);

		for(int i = 0; i < samples; i++) {
			buffer[i] /= 3;
		}
	}

	void triggerChannel1(void) { m_channel1.trigger(); }
	void triggerChannel2(void) { m_channel2.trigger(); }

	bool m_audioEnabled;
	uint8_t m_divApu = 0;

	Channel1 m_channel1;
	Channel2 m_channel2;
	Channel4 m_channel4;
};
