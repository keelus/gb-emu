#pragma once

#include "catridge.hpp"
#include <cstdint>
#include <cstring>

#define MEMORY_SIZE ((1 << 16) - 1)
#define ADDRESS_IS_CATRIDGE_ROM(address) ((address) < CATRIDGE_SIZE)

class Memory {
  public:
	Memory() { memset(m_data, 0, MEMORY_SIZE); }
	Memory(Catridge &catridge) {
		memset(m_data + CATRIDGE_SIZE, 0, MEMORY_SIZE - CATRIDGE_SIZE);
		memcpy(m_data, catridge.data(), CATRIDGE_SIZE);
	}

	uint8_t read8(const uint16_t address) const;
	void write8(const uint16_t address, const uint8_t value);
	void load(const uint16_t address, const uint8_t *buffer, size_t size) {
		if(address + size > MEMORY_SIZE) { throw std::runtime_error("Buffer to load in memory overflown."); }
		memcpy(m_data + address, buffer, size);
	}

  private:
	uint8_t m_data[MEMORY_SIZE];
};
