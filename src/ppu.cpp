#include "ppu.hpp"

void Bus::write8(const uint16_t address, const uint8_t value) {
	if(address >= 0x8000 && address <= 0x9FFF) {
		m_vram[address - 0x8000] = value;
	} else if(address >= 0xFE00 && address <= 0xFE9F) {
		m_oam[address - 0xFE00] = value;
	} else {
		std::stringstream stream;
		stream << "Ppu: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}

uint8_t Bus::read8(const uint16_t address) const {
	if(address >= 0x8000 && address <= 0x9FFF) {
		return m_vram[address - 0x8000];
	} else if(address >= 0xFE00 && address <= 0xFE9F) {
		return m_oam[address - 0xFE00];
	} else {
		std::stringstream stream;
		stream << "Ppu: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		throw std::runtime_error(stream.str());
	}
}
