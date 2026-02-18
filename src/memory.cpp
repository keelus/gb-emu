#include "memory.hpp"
#include <cstdint>

uint8_t Memory::read8(const uint16_t address) const {
	return m_data[address];
}

void Memory::write8(const uint16_t address, const uint8_t value) {
	if(ADDRESS_IS_CATRIDGE_ROM(address)) {
		throw std::runtime_error("Cannot write to ROM");
	} else {
		m_data[address] = value;
	}
}

uint16_t Memory::read16(const uint16_t address) const {
	return (static_cast<uint16_t>(m_data[address + 1]) << 8) | static_cast<uint16_t>(m_data[address]);
}

void Memory::write16(const uint16_t address, const uint16_t value) {
	write8(address, static_cast<uint8_t>(value));
	write8(address + 1, static_cast<uint8_t>(value >> 8));
}
