#pragma once

#include "bus.hpp"
#include "graphics/lcd.hpp"
#include <cstdint>
#include <optional>
#include <deque>

extern uint8_t activeColorPalette;
extern Color colorPalettes[3][4];

class BackgroundFifo {
  public:
	BackgroundFifo(Bus &bus, Lcd &lcd) : m_bus(bus), m_lcd(lcd) { reset(0, false); }

	void reset(const uint8_t scx, bool isWindow) {
		m_state = State::FetchTileNumber;
		m_dotsCurrentState = 0;
		m_dotsDone = 0;

		m_pixels.clear();

		m_xFetch = 0;

		m_pixelsRendered = 0;

		m_firstCopyPixelsRemaining = 8;

		m_pixelsOddDiscardRemaining = scx % 8;

		m_isWindow = isWindow;
	}

	void tickDot(const uint8_t ly, const uint8_t scy, const uint8_t scx, const uint8_t wy, const uint8_t wx,
				 const uint8_t wly) {
		if(m_lcd.screenX() >= Lcd::WIDTH) { return; }

		m_dotsCurrentState++;
		m_dotsDone++;

		switch(m_state) {
		case State::FetchTileNumber: {
			if(m_dotsCurrentState == 2 && !m_isWindow) {
				m_tileOffset = ((scx) / 8 + m_xFetch) & 0x1F;
				m_tileOffset += 32 * (((ly + scy) & 0xFF) / 8);
				m_tileOffset &= 0x3FF;

				m_state = State::FetchLow;
				m_dotsCurrentState = 0;
			} else if(m_dotsCurrentState == 2 && m_isWindow) {
				m_tileOffset = m_xFetch;
				m_tileOffset += 32 * (wly / 8);
				m_tileOffset &= 0x3FF;

				m_state = State::FetchLow;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchLow: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte1;
				size_t offset = 2 * ((ly + scy) % 8);
				if(m_isWindow) { offset = 2 * (wly % 8); }
				getTileHLine(m_tileOffset, offset, m_tileLow, byte1,
							 ((m_bus.read8(0xFF40) >> (m_isWindow ? 6 : 3)) & 1) != 0);

				m_state = State::FetchHigh;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchHigh: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte0;
				size_t offset = 2 * ((ly + scy) % 8);
				if(m_isWindow) { offset = 2 * (wly % 8); }
				getTileHLine(m_tileOffset, offset, byte0, m_tileHigh,
							 ((m_bus.read8(0xFF40) >> (m_isWindow ? 6 : 3)) & 1) != 0);

				m_state = State::Push;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::Push: {
			if(m_dotsCurrentState == 2) {
				if(m_pixels.empty()) {
					for(int i = 0; i < 8; i++) {
						uint8_t lower = (m_tileLow >> (7 - i)) & 1;
						uint8_t upper = (m_tileHigh >> (7 - i)) & 1;
						uint8_t colorId = (upper << 1) | lower;

						bool bgEnabled = (m_bus.read8(0xFF40)) & 0x1;
						if(bgEnabled) {
							m_pixels.push_back({colorId});
						} else {
							m_pixels.push_back({0});
						}
					}

					m_xFetch++;
					m_state = State::FetchTileNumber;
					m_dotsCurrentState = 0;
				} else {
					m_dotsCurrentState--;
				}
			}
			break;
		}
		}
	}

	struct BgPixel {
		Color color;
		bool isTransparent;
	};

	std::optional<BgPixel> pop() {
		if(m_pixels.empty() || m_lcd.screenX() >= Lcd::WIDTH) { return std::nullopt; }

		Pixel px = m_pixels.front();
		m_pixels.pop_front();

		uint8_t palette = m_bus.read8(0xFF47);
		uint8_t shade = (palette >> (px.color * 2)) & 0b11;

		if(m_firstCopyPixelsRemaining == 0) {
			if(m_pixelsOddDiscardRemaining == 0) {
				m_pixelsRendered++;
				return (BgPixel){
					.color = colorPalettes[activeColorPalette][shade],
					.isTransparent = px.color == 0,
				};
			} else {
				m_pixelsOddDiscardRemaining--;
			}
		} else {
			m_firstCopyPixelsRemaining--;
			m_xFetch = 0;
		}

		return std::nullopt;
	}

	void getTileHLine(uint16_t tileMapIndex, uint8_t offset, uint8_t &byte0, uint8_t &byte1,
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

		byte0 = m_bus.read8(finalTileAddress + offset);
		byte1 = m_bus.read8(finalTileAddress + offset + 1);
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

	uint8_t m_xFetch;

	uint16_t m_tileOffset = 0;

	uint8_t m_tileLow;

	uint8_t m_tileHigh;

	std::deque<Pixel> m_pixels;

	Bus &m_bus;
	Lcd &m_lcd;

	uint8_t m_firstCopyPixelsRemaining = 8;
	uint8_t m_pixelsOddDiscardRemaining = 0;

	bool m_started = false;

	uint8_t m_pixelsRendered = 0;

	uint8_t m_dotsCurrentState = 0;

	bool m_isWindow = false;
};
