#include "bus.hpp"
#include <cstdint>
#include <sstream>
#include <stdexcept>

#define IN_RANGE(address, start, end) ((address) >= (start) && (address) <= (end))
#define IS_CARTRIDGE(address) (IN_RANGE(address, 0x0000, 0x7FFF) || IN_RANGE(address, 0xA000, 0xBFFF))
#define IS_PPU(address) (IN_RANGE(address, 0x8000, 0x9FFF) || IN_RANGE(address, 0xFE00, 0xFE9F))
#define IS_MEMORY(address) (IN_RANGE(address, 0xC000, 0xFDFF) || IN_RANGE(address, 0xFF80, 0xFFFE))
#define IS_IO(address) (IN_RANGE(address, 0xFF00, 0xFF7F))
#define IS_IE(address) (address == 0xFFFF)

uint8_t Bus::read8(const uint16_t address) const {
	if(IS_CARTRIDGE(address)) {
		return m_cartridge->read8(address);
	} else if(IS_PPU(address)) {
		throw std::runtime_error("PPU not implemented.");
	} else if(IS_MEMORY(address)) {
		return m_memory->read8(address);
	} else if(IS_IO(address)) {
		return ioRead8(address);
	} else if(IS_IE(address)) {
		throw std::runtime_error("Interrupt Enable Register not implemented.");
	} else {
		std::stringstream stream;
		stream << "Bus: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

uint16_t Bus::read16(const uint16_t address) const {
	return (static_cast<uint16_t>(read8(address + 1)) << 8) | static_cast<uint16_t>(read8(address));
}

void Bus::write8(const uint16_t address, const uint8_t value) {
	if(IS_CARTRIDGE(address)) {
		m_cartridge->write8(address, value);
	} else if(IS_PPU(address)) {
		throw std::runtime_error("PPU not implemented.");
	} else if(IS_MEMORY(address)) {
		m_memory->write8(address, value);
	} else if(IS_IO(address)) {
		ioWrite8(address, value);
	} else if(IS_IE(address)) {
		throw std::runtime_error("Interrupt Enable Register not implemented.");
	} else {
		std::stringstream stream;
		stream << "Bus: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

void Bus::write16(const uint16_t address, const uint16_t value) {
	write8(address, static_cast<uint8_t>(value));
	write8(address + 1, static_cast<uint8_t>(value >> 8));
}


uint8_t Bus::ioRead8(const uint16_t address) const {
	switch(address) {
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
	}
}

void Bus::ioWrite8(const uint16_t address, const uint8_t value) {
	switch(address) {
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
			   << int(address) << std::endl;
		throw std::runtime_error(stream.str());
	}
	}
}
