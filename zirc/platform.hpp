#pragma once

#include <cstdint>

#include "common.hpp"

class GameBoy;

class Platform {
  public:
	void addGameBoy(GameBoy *gameBoy) { m_gameBoy = gameBoy; }
	void removeGameBoy() { m_gameBoy = nullptr; }

	virtual bool running() const = 0;

	virtual void beforeFrame() = 0;
	virtual void afterFrame() = 0;

	virtual void drawPixel(uint8_t x, uint8_t y, Color color) = 0;
	virtual void showFrame() = 0;
	virtual void swapBuffers() = 0;

	virtual float getAudioAmplitude() const = 0;
	virtual float getAudioSampleRate() const = 0;
	virtual void pushAudioSample(float sample) = 0;
	virtual void muteAudio() = 0;
	virtual void unmuteAudio() = 0;

  protected:
	GameBoy *m_gameBoy = nullptr;
};
