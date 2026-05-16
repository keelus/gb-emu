#pragma once

#include <cassert>
#include <cstdint>
#include <deque>

#include "../bus.hpp"
#include "lcd.hpp"

extern uint8_t activeColorPalette;
extern Color colorPalettes[3][4];

class SpriteFifo {
  public:
	SpriteFifo(Bus &bus) : m_bus(bus) { reset(0); }

	void reset(uint8_t fetchingSpriteIndex) {
		m_spriteIndex = fetchingSpriteIndex;
		m_state = State::FetchTileNumber;
		m_dotsCurrentState = 0;

		m_drawingBottom = false;
	}

	bool tickDot(const uint8_t ly) {
		m_dotsCurrentState++;

		uint8_t objSize = (m_bus.read8(0xFF40) >> 2) & 0x1;

		switch(m_state) {
		case State::FetchTileNumber: {
			if(m_dotsCurrentState == 2) {
				m_spriteY = m_bus.read8(static_cast<uint16_t>(0xFE00 + m_spriteIndex * 4 + 0)) - 16;
				m_spriteX = m_bus.read8(static_cast<uint16_t>(0xFE00 + m_spriteIndex * 4 + 1));
				m_tileIndex = m_bus.read8(static_cast<uint16_t>(0xFE00 + m_spriteIndex * 4 + 2));
				m_spriteAttrs = m_bus.read8(static_cast<uint16_t>(0xFE00 + m_spriteIndex * 4 + 3));

				bool flipY = (m_spriteAttrs & 0x40) == 0x40;
				m_localY = ly - m_spriteY;
				if(objSize) {
					m_drawingBottom = m_localY > 7;
					if(m_drawingBottom) { m_localY -= 8; }
					uint16_t newTileIndex =
						static_cast<uint16_t>((m_tileIndex &= 0xFE) | ((m_drawingBottom != flipY) ? 0x01 : 0));
					m_tileIndex = newTileIndex;
				}

				if(flipY) { m_localY = 7 - m_localY; }

				m_state = State::FetchLow;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchLow: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte1;
				getTileHLine(m_tileLow, byte1);

				m_state = State::FetchHigh;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::FetchHigh: {
			if(m_dotsCurrentState == 2) {
				uint8_t byte0;
				getTileHLine(byte0, m_tileHigh);

				m_state = State::Push;
				m_dotsCurrentState = 0;
			}
			break;
		}
		case State::Push: {
			if(m_dotsCurrentState == 2) {
				bool flipX = (m_spriteAttrs & 0x20) == 0x20;

				for(uint8_t i = 0; i < 8; i++) {
					uint8_t lower = (m_tileLow >> (flipX ? i : (7 - i))) & 1;
					uint8_t upper = (m_tileHigh >> (flipX ? i : (7 - i))) & 1;
					uint8_t colorId = static_cast<uint8_t>((upper << 1) | lower);

					uint8_t objEnable = (m_bus.read8(0xFF40) >> 1) & 0x1;
					if(!objEnable) { colorId = 0; }

					if(m_spriteX + i >= 8 && m_spriteX + i < Lcd::WIDTH + 8) {
						if(m_pixels.size() > i) {
							if(m_pixels.at(i).color == 0) {
								m_pixels.at(i) = {colorId, uint8_t(m_spriteX + i), ly, (m_spriteAttrs & 0x80) != 0};
							}
						} else {
							Pixel p(colorId, m_spriteX + i, ly, (m_spriteAttrs & 0x80) != 0);
							m_pixels.push_back(p);
						}
					}
				}

				m_state = State::FetchTileNumber;
				m_dotsCurrentState = 0;
				return true;
			}
			break;
		}
		}
		return false;
	}

	struct SpritePixel {
		Color color;
		bool behindBg;
		bool isTransparent;
	};

	bool pop(SpritePixel &spritePixel) {
		if(m_pixels.empty()) { return false; }

		Pixel px = m_pixels.front();
		m_pixels.pop_front();

		const uint8_t palette = (m_spriteAttrs & 0x10) ? m_bus.read8(0xFF49) : m_bus.read8(0xFF48);
		uint8_t shade = (palette >> (px.color * 2)) & 0b11;

		spritePixel.color = colorPalettes[activeColorPalette][shade];
		spritePixel.behindBg = px.behindBg;
		spritePixel.isTransparent = px.color == 0;

		return true;
	}

	void getTileHLine(uint8_t &byte0, uint8_t &byte1) const {
		uint16_t tileAddress = 0x8000 | (static_cast<uint16_t>(m_tileIndex) * 16);

		byte0 = m_bus.read8(static_cast<uint16_t>(tileAddress + m_localY * 2));
		byte1 = m_bus.read8(static_cast<uint16_t>(tileAddress + m_localY * 2 + 1));
	}

	enum class State {
		FetchTileNumber = 0,
		FetchLow = 1,
		FetchHigh = 2,
		Push = 3,
	};

  private:
	class Pixel {
	  public:
		Pixel(uint8_t _color, uint8_t _x, uint8_t _y, bool _behindBg) {
			color = _color;
			x = _x;
			y = _y;
			behindBg = _behindBg;
		}

		uint8_t color;
		uint8_t x;
		uint8_t y;
		bool behindBg;
	};

	State m_state;

	uint16_t m_tileIndex;
	uint8_t m_localY;
	uint8_t m_tileLow, m_tileHigh;

	std::deque<Pixel> m_pixels;

	Bus &m_bus;

	uint8_t m_dotsCurrentState = 0;

	uint8_t m_spriteX, m_spriteY, m_spriteAttrs;

	uint8_t m_spriteIndex;
	bool m_drawingBottom = false;
};
