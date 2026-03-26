#include "ppu.hpp"
#include "common.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <stdexcept>

uint32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};
uint32_t sdl2Buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = {0};

uint32_t colorPalettes[3][4] = {
	{0x00FFFFFF, 0x00AAAAAA, 0x00555555,		 0x0}, // Gray
	{0x009BBC0F, 0x008BAC0F, 0x00306230, 0x000F380F}, // CRT green
	{0x00E0C77F, 0x009D8B59, 0x005A5033, 0x0016140D}  // Yellow-ish
};

void updateSdl2Buffer() {
	memcpy(sdl2Buffer, buffer, sizeof(uint32_t) * (SCREEN_WIDTH * SCREEN_HEIGHT));
}

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
	m_cycles += cycles;

	switch(m_mode) {
	case PpuMode::OAM_SCAN: {
		m_requestedVblankInterrupt = false;
		m_requestedMode0Interrupt = false;
		m_requestedMode1Interrupt = false;

		if((m_lcdStatus & 0b100000) != 0 && !m_requestedMode2Interrupt) {
			m_requestedMode2Interrupt = true;
			m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		}

		if(m_cycles >= 80) {
			m_cycles -= 80;
			m_mode = PpuMode::DRAWING;
		}
		break;
	}
	case PpuMode::DRAWING: {
		m_requestedVblankInterrupt = false;
		m_requestedMode0Interrupt = false;
		m_requestedMode1Interrupt = false;
		m_requestedMode2Interrupt = false;
		if(m_cycles >= 172) {
			m_cycles -= 172;
			m_mode = PpuMode::HBLANK;
		}
		break;
	}
	case PpuMode::HBLANK: {
		m_requestedVblankInterrupt = false;
		m_requestedMode1Interrupt = false;
		m_requestedMode2Interrupt = false;
		if((m_lcdStatus & 0b1000) != 0 && !m_requestedMode0Interrupt) {
			m_requestedMode0Interrupt = true;
			m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		}
		if(m_cycles >= 204) {
			m_cycles -= 204;

			drawHLine();
			/* TODO: Draw Window */
			if((m_lcdStatus & 0b100) != 0 && m_ly == m_lyc) { m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd); }

			m_ly++;

			if(m_ly == 144) {
				m_mode = PpuMode::VBLANK;
			} else {
				m_mode = PpuMode::OAM_SCAN;
			}
		}
		break;
	}
	case PpuMode::VBLANK: {
		m_requestedVblankInterrupt = false;
		m_requestedMode0Interrupt = false;
		m_requestedMode2Interrupt = false;

		if((m_lcdStatus & 0b10000) != 0 && !m_requestedMode1Interrupt) {
			m_requestedMode1Interrupt = true;
			m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
		}

		if(!m_requestedVblankInterrupt) {
			m_bus.requestInterrupt(Bus::InterruptRequestType::VBlank);
			m_requestedVblankInterrupt = true;
		}

		if(m_cycles >= 456) {
			if((m_lcdStatus & 0x0b100) != 0 && m_ly == m_lyc) {
				m_bus.requestInterrupt(Bus::InterruptRequestType::Lcd);
			}
			m_ly++;
			m_cycles -= 456;

			if(m_ly == 154) {
				drawObjects();
				updateSdl2Buffer();

				m_ly = 0;
				m_mode = PpuMode::OAM_SCAN;
			}
		}
		break;
	}
	}

	m_lcdStatus =
		(m_lcdStatus & 0xF8) | (static_cast<uint8_t>(m_lyc == m_ly) << 2) | (static_cast<uint8_t>(m_mode) & 0x3);
}

void Ppu::drawHLine() const {
	size_t y = m_ly + m_scy;
	size_t tileI = y / 8;

	for(size_t x = m_scx; x < 160 + m_scx; x++) {
		size_t tileJ = x / 8;
		size_t tileIndex = tileI * 32 + tileJ;

		uint8_t byte0, byte1;
		int localX = abs(int(x - tileJ * 8));
		int localY = abs(int(y - tileI * 8));
		getTileHLine(tileIndex, localY, byte0, byte1);
		drawTileHLine(localX, x - m_scx, y - m_scy, byte0, byte1);
	}
}

void Ppu::getTileHLine(uint16_t tileMapIndex, uint8_t desiredI, uint8_t &byte0, uint8_t &byte1) const {
	bool lcdc4 = ((m_bus.read8(0xFF40) >> 4) & 1) != 0;
	bool lcdc3 = ((m_bus.read8(0xFF40) >> 3) & 1) != 0;

	uint16_t tileAddress = lcdc3 ? 0x9C00 : 0x9800;
	uint8_t index = m_bus.read8(tileAddress + tileMapIndex);

	uint16_t finalTileAddress;

	if(lcdc4) {
		finalTileAddress = 0x8000 + index * 16;
	} else {
		finalTileAddress = 0x9000 + (int8_t)index * 16;
	}

	byte0 = m_bus.read8(finalTileAddress + desiredI * 2);
	byte1 = m_bus.read8(finalTileAddress + desiredI * 2 + 1);
}

void Ppu::drawPixel(uint8_t i, uint8_t j, uint32_t color) const {
	buffer[SCREEN_WIDTH * i + j] = color;
}

void Ppu::drawTileHLine(uint8_t localX, uint8_t x, uint8_t y, uint8_t byte0, uint8_t byte1) const {
	if(x >= 160) { return; }
	if(y >= 144) { return; }

	uint8_t lower = byte0 >> (7 - localX) & 1;
	uint8_t upper = byte1 >> (7 - localX) & 1;
	uint8_t colorId = (upper << 1) | lower;

	uint8_t palette = m_bus.read8(0xFF47);
	uint8_t shade = (palette >> (colorId * 2)) & 0b11;
	drawPixel(y, x, colorPalettes[activeColorPalette][shade]);
}

// TODO: Add priority, flipping, 8x16 mode, etc.
void Ppu::drawObjects(void) const {
	for(size_t i = 0; i < 40; i++) {
		uint8_t yPos = m_bus.read8(0xFE00 | static_cast<uint16_t>(i * 4 + 0));
		uint8_t xPos = m_bus.read8(0xFE00 | static_cast<uint16_t>(i * 4 + 1));

		if(yPos == 0 || yPos >= 160 || xPos == 0 || xPos >= 168) { continue; }

		yPos -= 16;
		xPos -= 8;

		uint8_t tileIndex = m_bus.read8(0xFE00 | static_cast<uint16_t>(i * 4 + 2));
		uint16_t tileAddress = 0x8000 | (static_cast<uint16_t>(tileIndex) * 16);

		uint8_t attributes = m_bus.read8(0xFE00 | static_cast<uint16_t>(i * 4 + 3));

		for(size_t localY = 0; localY < 8; localY++) {
			uint8_t byte0 = m_bus.read8(tileAddress + localY * 2);
			uint8_t byte1 = m_bus.read8(tileAddress + localY * 2 + 1);
			for(size_t localX = 0; localX < 8; localX++) {
				uint8_t lower = byte0 >> (7 - localX) & 1;
				uint8_t upper = byte1 >> (7 - localX) & 1;

				uint8_t colorId = (upper << 1) | lower;
				if(colorId == 0) { continue; }

				uint8_t palette = (attributes & 0x10) ? m_objPalette1 : m_objPalette0;
				uint8_t shade = (palette >> (colorId * 2)) & 0b11;
				drawPixel(yPos + localY, xPos + localX, colorPalettes[activeColorPalette][shade]);
			}
		}
	}
}
