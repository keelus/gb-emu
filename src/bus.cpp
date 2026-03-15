#include "bus.hpp"
#include "ppu.hpp"
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
		return m_ppu->read8(address);
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
		return m_ppu->write8(address, value);
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
	case 0xFF10:
	case 0xFF11:
	case 0xFF12:
	case 0xFF13:
	case 0xFF14:
	case 0xFF15:
	case 0xFF16:
	case 0xFF17:
	case 0xFF18:
	case 0xFF19:
	case 0xFF1A:
	case 0xFF1B:
	case 0xFF1C:
	case 0xFF1D:
	case 0xFF1E:
	case 0xFF1F:
	case 0xFF20:
	case 0xFF21:
	case 0xFF22:
	case 0xFF23:
	case 0xFF24:
	case 0xFF25:
	case 0xFF26: return m_audioMem[address - 0xFF10]; break;
	case 0xFF40: return m_ppu->getControl(); break;
	case 0xFF42: return m_ppu->getScy(); break;
	case 0xFF44: return m_ppu->getLy(); break;
	case 0xFF47: return m_ppu->getPalette(); break;
	case 0xFF46: throw std::runtime_error("Bus: Unhandled read to 0xFF46. OAM not implemented."); break;
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << int(address)
			   << std::endl;
		std::cout << "WARNING: " << stream.str();
		return m_iomem[address - 0xFF00];
	}
	}
}

void Bus::ioWrite8(const uint16_t address, const uint8_t value) {
	switch(address) {
	case 0xFF10:
	case 0xFF11:
	case 0xFF12:
	case 0xFF13:
	case 0xFF14:
	case 0xFF15:
	case 0xFF16:
	case 0xFF17:
	case 0xFF18:
	case 0xFF19:
	case 0xFF1A:
	case 0xFF1B:
	case 0xFF1C:
	case 0xFF1D:
	case 0xFF1E:
	case 0xFF1F:
	case 0xFF20:
	case 0xFF21:
	case 0xFF22:
	case 0xFF23:
	case 0xFF24:
	case 0xFF25:
	case 0xFF26: m_audioMem[address - 0xFF10] = value; break;
	case 0xFF40: m_ppu->setControl(value); break;
	case 0xFF42: m_ppu->setScy(value); break;
	case 0xFF47: m_ppu->setPalette(value); break;
	case 0xFF46: throw std::runtime_error("Bus: Unhandled write to 0xFF46. OAM not implemented."); break;
	default: {
		m_iomem[address - 0xFF00] = value;

		std::stringstream stream;
		stream << "Bus: Illegal I/O write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
			   << int(address) << std::endl;
		std::cout << "WARNING: " << stream.str();
	}
	}
}
