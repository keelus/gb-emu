#pragma once

#include "bus.hpp"
#include <cstdint>
#include <cstring>

#define PPU_VRAM_SIZE 8192
#define PPU_OAM_SIZE 160

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
extern uint32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

class Ppu {
  public:
	enum class PpuMode { OAM_SCAN, DRAWING, HBLANK, VBLANK };
	Ppu(Bus &bus) : m_bus(bus) {
		m_cycles = 0;
		m_mode = PpuMode::OAM_SCAN;

		m_control = 0;

		m_palette = 0;
		m_scx = 0;
		m_scy = 0;

		m_ly = 0;

		memset(m_vram, 0, sizeof(uint8_t) * PPU_OAM_SIZE);
		memset(m_oam, 0, sizeof(uint8_t) * PPU_OAM_SIZE);
	}

	void tick(uint8_t cycles);

	uint8_t getControl() const { return m_control; }
	void setControl(const uint8_t newControl) { m_control = newControl; }

	uint8_t getPalette() const { return m_palette; }
	void setPalette(const uint8_t newPalette) { m_palette = newPalette; }

	uint8_t getScy() const { return m_scy; }
	void setScy(const uint8_t newScy) { m_scy = newScy; }

	uint8_t getLy() const { return m_ly; }

	void write8(const uint16_t address, const uint8_t value);
	uint8_t read8(const uint16_t address) const;

  private:
	void getTileHLine(uint16_t tileMapIndex, uint8_t desiredI, uint8_t &byte0, uint8_t &byte1) const;
	void drawHLine();
	void drawTileHLine(uint8_t localX, uint8_t x, uint8_t y, uint8_t byte0, uint8_t byte1) const;
	void drawPixel(uint8_t i, uint8_t j, uint32_t color) const;

	uint16_t m_cycles;

	uint8_t m_vram[PPU_VRAM_SIZE];
	uint8_t m_oam[PPU_OAM_SIZE];

	uint8_t m_control;
	uint8_t m_palette;

	uint8_t m_scy, m_scx;

	uint8_t m_ly;

	PpuMode m_mode;

	Bus &m_bus;
};
