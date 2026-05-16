#pragma once

#include <cassert>
#include <cstdint>
#include <sys/types.h>

#include "../platform.hpp"

namespace Zirc {
class Lcd {
  public:
	Lcd(Platform &platform) : m_platform(platform) {}

	void drawPixel(uint8_t y, Color color) {
		if(y >= HEIGHT || m_screenX >= WIDTH) { return; }
		m_platform.drawPixel(m_screenX, y, color);
		m_screenX++;
	}

	uint8_t screenX() const { return m_screenX; }
	void resetScreenX() { m_screenX = 0; }

	void showBuffer() { m_platform.swapBuffers(); }

	static constexpr size_t WIDTH = 160;
	static constexpr size_t HEIGHT = 144;

  private:
	uint8_t m_screenX = 0;
	Platform &m_platform;
};
} // namespace Zirc
