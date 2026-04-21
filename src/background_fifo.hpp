#pragma once

#include "bus.hpp"
#include <cstdint>
#include <deque>

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
extern uint32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
extern uint8_t activeColorPalette;
extern uint32_t colorPalettes[3][4];

class BackgroundFifo {
  public:
	BackgroundFifo(Bus &bus) : m_bus(bus) { reset(0); }

	void reset(const uint8_t scx) {
		std::cout << "Last scanline took " << std::dec << int(m_dotsDone) << " dots" << std::endl;
		m_state = State::FetchTileNumber;
		m_dotsCurrentState = 0;
		m_dotsDone = 0;

		m_pixels.clear();

		m_xFetch = 0;
		m_xScreen = 0;

		m_pixelsRendered = 0;

		m_firstCopyPixelsRemaining = 8;

		m_pixelsOddDiscardRemaining = scx % 8;
	}

	void tickDot(const uint8_t ly, const uint8_t scy, const uint8_t scx) {
		if(m_xScreen >= 160) { return; }

		m_dotsCurrentState++;
		m_dotsDone++;

		switch(m_state) {
		case State::FetchTileNumber: {
			if(m_dotsCurrentState == 2) {
				size_t y = (ly + scy) % 256;
				size_t x = (m_xFetch + scx) % 256;

				size_t tileI = y / 8;
				size_t tileJ = x / 8;

				m_localY = abs(int(y - tileI * 8));
				m_localX = abs(int(x - tileJ * 8));

				m_tileIndex = tileI * 32 + tileJ;

				m_state = State::FetchLow;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchLow: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte1;
				getTileHLine(m_tileIndex, m_localY, m_tileLow, byte1, ((m_bus.read8(0xFF40) >> 3) & 1) != 0);

				m_state = State::FetchHigh;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchHigh: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte0;
				getTileHLine(m_tileIndex, m_localY, byte0, m_tileHigh, ((m_bus.read8(0xFF40) >> 3) & 1) != 0);

				m_state = State::Push;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::Push: {
			if(m_pixels.empty()) {
				for(int i = 0; i < 8; i++) {
					uint8_t lower = (m_tileLow >> (7 - i)) & 1;
					uint8_t upper = (m_tileHigh >> (7 - i)) & 1;
					uint8_t colorId = (upper << 1) | lower;

					m_pixels.push_back({colorId});
				}

				m_xFetch += 8;
				m_state = State::FetchTileNumber;
				m_dotsCurrentState = 0;
			}
			break;
		}
		}

		popIfPossible(ly);
	}

	void popIfPossible(const uint8_t y) {
		if(m_pixels.empty() || m_xScreen >= 160) { return; }

		Pixel px = m_pixels.front();
		m_pixels.pop_front();

		uint8_t palette = m_bus.read8(0xFF47);
		uint8_t shade = (palette >> (px.color * 2)) & 0b11;

		if(m_firstCopyPixelsRemaining == 0) {
			if(m_pixelsOddDiscardRemaining == 0) {
				drawPixel(y, m_xScreen, colorPalettes[activeColorPalette][shade]);
				m_pixelsRendered++;
				m_xScreen++;
			} else {
				m_pixelsOddDiscardRemaining--;
			}
		} else {
			m_firstCopyPixelsRemaining--;
			m_xFetch = 0;
		}
	}

	void drawPixel(uint8_t i, uint8_t j, uint32_t color) const {
		if(i >= 144 || j >= 160) { return; }
		buffer[SCREEN_WIDTH * i + j] = color;
	}

	void getTileHLine(uint16_t tileMapIndex, uint8_t desiredI, uint8_t &byte0, uint8_t &byte1,
					  uint8_t tileAddressBit) const {
		uint16_t tileAddress = tileAddressBit ? 0x9C00 : 0x9800;
		uint8_t index = m_bus.read8(tileAddress + tileMapIndex);

		uint16_t finalTileAddress;

		uint8_t tileBit = ((m_bus.read8(0xFF40) >> 4) & 1) != 0;
		if(tileBit) {
			finalTileAddress = 0x8000 + index * 16;
		} else {
			finalTileAddress = 0x9000 + (int8_t)index * 16;
		}

		byte0 = m_bus.read8(finalTileAddress + desiredI * 2);
		byte1 = m_bus.read8(finalTileAddress + desiredI * 2 + 1);
	}

	struct Pixel {
		uint8_t color;
	};

	enum class State {
		FetchTileNumber = 0,
		FetchLow = 1,
		FetchHigh = 2,
		Push = 3,
	};

	uint8_t pixelsRendered() const { return m_pixelsRendered; }

  private:
	State m_state;
	uint16_t m_dotsDone = 0;

	uint8_t m_xScreen;
	uint8_t m_xFetch;

	uint16_t m_tileIndex;
	uint8_t m_localY, m_localX;

	uint8_t m_tileLow;

	uint8_t m_tileHigh;

	std::deque<Pixel> m_pixels;

	Bus &m_bus;

	uint8_t m_firstCopyPixelsRemaining = 8;
	uint8_t m_pixelsOddDiscardRemaining = 0;

	bool m_started = false;

	uint8_t m_pixelsRendered = 0;

	uint8_t m_dotsCurrentState = 0;
};
