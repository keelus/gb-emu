#pragma once

#include "catridge.hpp"
#include <cstdint>
#include <cstring>

#define MEMORY_SIZE ((1 << 16) - 1)
#define ADDRESS_IS_CATRIDGE_ROM(address) ((address) < CATRIDGE_SIZE)

class Memory {
  public:
	Memory(Catridge &catridge) : m_catridge(catridge) { memset(m_data, 0, MEMORY_SIZE); }

	uint8_t read8(const uint16_t address) const;
	void write8(const uint16_t address, const uint8_t value);

  private:
	uint8_t m_data[MEMORY_SIZE];
	Catridge &m_catridge;
};
