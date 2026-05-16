#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <zirc/audio/ringbuffer.hpp>
#include <zirc/config.hpp>
#include <zirc/gameboy.hpp>
#include <zirc/joypad.hpp>
#include <zirc/platform.hpp>

#define SCALE 5
#define FRAME_BUFFER_SIZE (Lcd::WIDTH * Lcd::HEIGHT)

extern uint8_t activeColorPalette;

class PlatformHeadless : public Platform {
  public:
	bool running() const override { return m_running; }
	void drawPixel(uint8_t x, uint8_t y, Color color) override {}

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
	uint8_t m_nextActiveColorPalette = activeColorPalette;

	bool m_running = true;
	bool m_limitFps = true;

	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr size_t AUDIO_SAMPLE_AMOUNT = 1024;
};
