#pragma once

#include "bus.hpp"
#include <cstdint>
#include <cstring>

#define PPU_VRAM_SIZE 8192
#define PPU_OAM_SIZE 160

#define SCREEN_WIDTH 160
#define SCREEN_HEIGHT 144
extern uint32_t buffer[SCREEN_WIDTH * SCREEN_HEIGHT];
extern uint32_t sdl2Buffer[SCREEN_WIDTH * SCREEN_HEIGHT];

extern uint8_t activeColorPalette;

class Ppu {
  public:
	enum class PpuMode { OAM_SCAN = 2, DRAWING = 3, HBLANK = 0, VBLANK = 1 };
	Ppu(Bus &bus) : m_bus(bus) {
		m_cycles = 0;
		m_mode = PpuMode::OAM_SCAN;

		m_control = 0;

		m_palette = 0;
		m_scx = 0;
		m_scy = 0;

		m_ly = 0;
		m_lyc = 0;

		m_requestedVblankInterrupt = false;

		memset(m_vram, 0, sizeof(m_vram));
		memset(m_oam, 0, sizeof(m_oam));

		m_lcdStatus = 0;
	}

	void tick(uint8_t cycles);

	uint8_t getControl() const { return m_control; }
	void setControl(const uint8_t newControl) { m_control = newControl; }

	uint8_t getPalette() const { return m_palette; }
	void setPalette(const uint8_t newPalette) { m_palette = newPalette; }

	uint8_t getObjPalette0() const { return m_objPalette0; }
	void setObjPalette0(const uint8_t newPalette) { m_objPalette0 = newPalette; }

	uint8_t getObjPalette1() const { return m_objPalette1; }
	void setObjPalette1(const uint8_t newPalette) { m_objPalette1 = newPalette; }

	uint8_t getScx() const { return m_scx; }
	void setScx(const uint8_t newScx) { m_scx = newScx; }

	uint8_t getScy() const { return m_scy; }
	void setScy(const uint8_t newScy) { m_scy = newScy; }

	uint8_t getWx() const { return m_wx; }
	void setWx(const uint8_t newWx) { m_wx = newWx; }

	uint8_t getWy() const { return m_wy; }
	void setWy(const uint8_t newWy) { m_wy = newWy; }

	uint8_t getLy() const { return m_ly; }

	void setLyc(const uint8_t newLyc) { m_lyc = newLyc; }
	uint8_t getLyc() const { return m_ly; }

	void write8(const uint16_t address, const uint8_t value);
	uint8_t read8(const uint16_t address) const;

	uint8_t getLcdStatus() const { return m_lcdStatus; }
	void setLcdStatus(const uint8_t newLcdStatus) {
		if(newLcdStatus != 0) {
			std::cout << "LCD_STATUS = 0x" << std::hex << std::setw(2) << std::setfill('0') << uint(newLcdStatus)
					  << std::endl;
		}
		m_lcdStatus = (newLcdStatus & 0xF8) | (m_lcdStatus & 0x7);
	}

  private:
	void drawObjects(void) const;
	void getTileHLine(uint16_t tileMapIndex, uint8_t desiredI, uint8_t &byte0, uint8_t &byte1) const;
	void drawHLine() const;
	void drawTileHLine(uint8_t localX, uint8_t x, uint8_t y, uint8_t byte0, uint8_t byte1) const;
	void drawPixel(uint8_t i, uint8_t j, uint32_t color) const;

	uint16_t m_cycles;

	uint8_t m_vram[PPU_VRAM_SIZE];
	uint8_t m_oam[PPU_OAM_SIZE];

	uint8_t m_control;
	uint8_t m_palette;

	uint8_t m_objPalette0, m_objPalette1;

	uint8_t m_scx, m_scy;
	uint8_t m_wx, m_wy;

	uint8_t m_ly, m_lyc;

	PpuMode m_mode;

	uint8_t m_lcdStatus;

	bool m_requestedVblankInterrupt;

	bool m_requestedMode0Interrupt;
	bool m_requestedMode1Interrupt;
	bool m_requestedMode2Interrupt;

	Bus &m_bus;
};
