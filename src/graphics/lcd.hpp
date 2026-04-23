#pragma once

#include <cassert>
#include <cstdint>
#include <cstring>
#include <sys/types.h>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144

class Lcd {
  public:
	const uint32_t *getBuffer() const { return m_buffer; }
	void showBuffer() { std::memcpy(m_buffer, m_temporalBuffer, sizeof(m_buffer)); }

	void drawPixel(uint8_t x, uint8_t y, uint32_t color) {
		if(y >= 144 || x >= 160) { return; }
		m_temporalBuffer[SCREEN_WIDTH * y + x] = color;
	}

  private:
	uint32_t m_temporalBuffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
	uint32_t m_buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
};
