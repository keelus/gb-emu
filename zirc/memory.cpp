#include <cstdint>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include "common.hpp"
#include "memory.hpp"

uint8_t Memory::read8(const uint16_t address) const {
	if(IN_RANGE(address, 0xC000, 0xDFFF)) {
		return m_wram[address - 0xC000];
	} else if(IN_RANGE(address, 0xFF80, 0xFFFE)) {
		return m_hram[address - 0xFF80];
	} else if(IN_RANGE(address, 0xE000, 0xFDFF)) {
		return m_wram[address - 0xE000];
	} else {
		std::stringstream stream;
		stream << "Memory: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

void Memory::write8(const uint16_t address, const uint8_t value) {
	if(IN_RANGE(address, 0xC000, 0xDFFF)) {
		m_wram[address - 0xC000] = value;
	} else if(IN_RANGE(address, 0xFF80, 0xFFFE)) {
		m_hram[address - 0xFF80] = value;
	} else if(IN_RANGE(address, 0xE000, 0xFDFF)) {
		m_wram[address - 0xE000] = value;
	} else {
		std::stringstream stream;
		stream << "Memory: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

uint16_t Memory::read16(const uint16_t address) const {
	return static_cast<uint16_t>((read8(address + 1) << 8) | read8(address));
}

void Memory::write16(const uint16_t address, const uint16_t value) {
	write8(address, static_cast<uint8_t>(value));
	write8(address + 1, static_cast<uint8_t>(value >> 8));
}
