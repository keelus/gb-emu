#include "ppu.hpp"
#include "common.hpp"
#include "graphics/background_fifo.hpp"
#include "graphics/lcd.hpp"
#include "graphics/sprite_fifo.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <stdexcept>

uint32_t colorPalettes[3][4] = {
	{0x00FFFFFF, 0x00AAAAAA, 0x00555555,		 0x0}, // Gray
	{0x009BBC0F, 0x008BAC0F, 0x00306230, 0x000F380F}, // CRT green
	{0x00E0C77F, 0x009D8B59, 0x005A5033, 0x0016140D}  // Yellow-ish
};

void Ppu::write8(const uint16_t address, const uint8_t value) {
	if(IN_RANGE(address, 0x8000, 0x9FFF)) {
		m_vram[address - 0x8000] = value;
	} else if(IN_RANGE(address, 0xFE00, 0xFE9F)) {
		m_oam[address - 0xFE00] = value;
	} else {
		std::stringstream stream;
		stream << "Ppu: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

uint8_t Ppu::read8(const uint16_t address) const {
	if(IN_RANGE(address, 0x8000, 0x9FFF)) {
		return m_vram[address - 0x8000];
	} else if(IN_RANGE(address, 0xFE00, 0xFE9F)) {
		return m_oam[address - 0xFE00];
	} else {
		std::stringstream stream;
		stream << "Ppu: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

void Ppu::tick(const uint8_t cycles) {
	if((m_control & 0b10000000) == 0) { return; }

	for(size_t i = 0; i < cycles; i++) {
		tickDot();
	}
}

void Ppu::tickDot() {
	m_cycles++;
	switch(m_mode) {
	case PpuMode::OAM_SCAN: {
		if(m_cycles >= 80) {
			m_cycles -= 80;
			m_mode = PpuMode::DRAWING;
			m_backgroundFifo.reset(m_scx, false);
			m_spriteFifo.reset(0);
			m_fetchingSprites = false;
			m_drawingWindow = false;
			m_lcd.resetScreenX();

			m_spritesToDraw.clear();
			for(size_t i = 0; i < 40; i++) {
				uint8_t y = m_bus.read8(0xFE00 + i * 4);
				if(y == 0) { continue; }
				if(y >= SCREEN_HEIGHT + 16) { continue; }

				uint8_t x = m_bus.read8(0xFE00 + i * 4 + 1);
				if(x == 0) { continue; }
				if(x >= SCREEN_WIDTH + 8) { continue; }

				uint8_t objSize = (m_bus.read8(0xFF40) >> 2) & 0x1;
				uint8_t spriteHeight = objSize == 1 ? 16 : 8;
				if(m_ly + 16 >= y && m_ly + 16 < y + spriteHeight) { m_spritesToDraw.push_back({uint8_t(i), x}); }

				if(m_spritesToDraw.size() == 10) { break; }
			}
		}
		break;
	}
	case PpuMode::DRAWING: {
		if(checkSpritesToDraw()) { m_spriteFifo.reset(m_fetchingSpriteIndex); }

		bool windowEnabled = (m_bus.read8(0xFF40) >> 5) & 0x1;
		if(!m_drawingWindow && windowEnabled && m_yCondition && m_lcd.screenX() >= m_wx - 7) {
			m_drawingWindow = true;
			m_backgroundFifo.reset(0, true);
		}

		if(m_fetchingSprites) {
			bool finished = m_spriteFifo.tickDot(m_ly);
			if(finished) {
				m_fetchingSprites = false;
			} else {
				return;
			}
		} else {
			m_backgroundFifo.tickDot(m_ly, m_scy, m_scx, m_wy, m_wx, m_wly);
		}

		std::optional<BackgroundFifo::BgPixel> bgPx = m_backgroundFifo.pop();
		if(bgPx.has_value()) {
			std::optional<SpriteFifo::SpritePixel> spritePx = m_spriteFifo.pop();

			if(!spritePx.has_value() || spritePx->isTransparent || (spritePx->behindBg && !bgPx->isTransparent)) {
				m_lcd.drawPixel(m_ly, bgPx->color);
			} else {
				m_lcd.drawPixel(m_ly, spritePx->color);
			}
		}

		if(m_lcd.screenX() >= 160) {
			m_cycles = 0;
			m_mode = PpuMode::HBLANK;
			m_m0StatRequested = false;
		}
		break;
	}
	case PpuMode::HBLANK: {
		if(m_cycles >= 256) {
			m_cycles -= 204;

			if(m_drawingWindow) { m_wly++; }

			m_ly++;
			if(!m_yCondition) { m_yCondition = m_wy == m_ly; }
			m_lycStatRequested = false;

			if(m_ly == 144) {
				m_mode = PpuMode::VBLANK;
				m_wly = 0;
				m_bus.requestInterrupt(Bus::InterruptRequestType::VBlank);
				m_m1StatRequested = false;
			} else {
				m_mode = PpuMode::OAM_SCAN;
				m_m2StatRequested = false;
			}
		}
		break;
	}
	case PpuMode::VBLANK: {
		m_drawingWindow = false;
		if(m_cycles >= 456) {
			m_ly++;
			if(!m_yCondition) { m_yCondition = m_wy == m_ly; }
			m_lycStatRequested = false;
			m_cycles -= 456;

			if(m_ly == 154) {
				m_lcd.showBuffer();
				m_ly = 0;
				m_yCondition = false;
				m_mode = PpuMode::OAM_SCAN;
				m_m2StatRequested = false;
			}
		}
		break;
	}
	}

	if(!m_lycStatRequested && (m_lcdStatus & 0b1000000) != 0 && m_ly == m_lyc) {
		m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		m_lycStatRequested = true;
	}
	if(!m_m2StatRequested && (m_lcdStatus & 0b100000) && m_mode == PpuMode::OAM_SCAN && m_mode != m_prevMode) {
		m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		m_m2StatRequested = true;
	}
	if(!m_m1StatRequested && (m_lcdStatus & 0b10000) && m_mode == PpuMode::VBLANK && m_mode != m_prevMode) {
		m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		m_m1StatRequested = true;
	}
	if(!m_m0StatRequested && (m_lcdStatus & 0b1000) && m_mode == PpuMode::HBLANK && m_mode != m_prevMode) {
		m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		m_m0StatRequested = true;
	}
}

bool Ppu::checkSpritesToDraw() {
	if(m_fetchingSprites) { return false; }
	for(size_t i = 0; i < m_spritesToDraw.size(); i++) {
		if(m_spritesToDraw.at(i).x <= m_lcd.screenX() + 8) {
			m_fetchingSprites = true;
			m_fetchingSpriteIndex = m_spritesToDraw.at(i).index;
			m_spritesToDraw.erase(m_spritesToDraw.begin() + i);
			return true;
		}
	}

	return false;
}
