#include "memory.hpp"
#include <stdexcept>

uint8_t Memory::read8(const uint16_t address) const {
	if(ADDRESS_IS_CATRIDGE_ROM(address)) {
		return m_catridge.read8(address);
	} else {
		return m_data[address];
	}
}

void Memory::write8(const uint16_t address, const uint8_t value) {
	if(ADDRESS_IS_CATRIDGE_ROM(address)) {
		throw std::runtime_error("Cannot write to ROM");
	} else {
		m_data[address] = value;
	}
}
