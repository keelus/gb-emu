#pragma once

#include <cassert>
#include <cstdint>

#include <zirc/audio/ringbuffer.hpp>
#include <zirc/config.hpp>
#include <zirc/gameboy.hpp>
#include <zirc/joypad.hpp>
#include <zirc/platform.hpp>

class PlatformHeadless : public Zirc::Platform {
  public:
	bool running() const override { return m_running; }
	void drawPixel(uint8_t x, uint8_t y, Zirc::Color color) override {}

	void beforeFrame() override {}
	void afterFrame() override {}
	void swapBuffers() override {}
	void showFrame() override {}

	float getAudioAmplitude() const override { return 1; }
	float getAudioSampleRate() const override { return AUDIO_SAMPLE_RATE; }

	void pushAudioSample(float sample) override {}

	void muteAudio() override {}
	void unmuteAudio() override {}

  private:
	uint8_t m_nextActiveColorPalette = Zirc::Config::get().activeColorPalette;

	bool m_running = true;

	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
};
