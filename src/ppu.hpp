#pragma once

#include "bus.hpp"
#include <cstdint>

#define PPU_VRAM_SIZE 8192
#define PPU_OAM_SIZE 160

class Ppu {
  public:
	Ppu(Bus &bus) : m_bus(bus) {
		m_cycles = 0;
		m_state = V_BLANK;

		memset(m_vram, 0, sizeof(uint8_t) * PPU_OAM_SIZE);
		memset(m_oam, 0, sizeof(uint8_t) * PPU_OAM_SIZE);
	}

	void tick(uint8_t cycles) {
		m_cycles += cycles;
		// TODO
	}

	void write8(const uint16_t address, const uint8_t value);
	uint8_t read8(const uint16_t address) const;

  private:
	uint16_t m_cycles;

	uint8_t m_vram[PPU_VRAM_SIZE];
	uint8_t m_oam[PPU_OAM_SIZE];

	Bus &m_bus;
};
