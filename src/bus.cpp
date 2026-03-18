#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "common.hpp"
#include "ppu.hpp"
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#define IS_CARTRIDGE(address) (IN_RANGE(address, 0x0000, 0x7FFF) || IN_RANGE(address, 0xA000, 0xBFFF))
#define IS_PPU(address) (IN_RANGE(address, 0x8000, 0x9FFF) || IN_RANGE(address, 0xFE00, 0xFE9F))
#define IS_MEMORY(address) (IN_RANGE(address, 0xC000, 0xFDFF) || IN_RANGE(address, 0xFF80, 0xFFFE))
#define IS_IO(address) (IN_RANGE(address, 0xFF00, 0xFF7F))
#define IS_IE(address) (address == 0xFFFF)
#define IS_FORBIDDEN(address) (IN_RANGE(address, 0xFEA0, 0xFEFF))

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
		return m_cpu->getInterruptFlagRaw(); // Is this readable?
	} else if(IS_FORBIDDEN(address)) {
		std::cout << "Bus: Ignoring read on forbidden address 0x" << std::hex << std::setw(4) << std::setfill('0')
				  << uint(address) << std::endl;
		return 0x00;
	} else {
		std::stringstream stream;
		stream << "Bus: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0') << uint(address)
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
		m_cpu->setInterruptFlagRaw(address);
	} else if(IS_FORBIDDEN(address)) {
		std::cout << "Bus: Ignoring write on forbidden address 0x" << std::hex << std::setw(4) << std::setfill('0')
				  << uint(address) << std::endl;
	} else {
		std::stringstream stream;
		stream << "Bus: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0') << uint(address)
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
	case 0xff01:
	case 0xff02: /* Ignore serial transfers */ break;
	case 0xFF10:
	case 0xFF11:
	case 0xFF12:
	case 0xFF14:
	case 0xFF15:
	case 0xFF16:
	case 0xFF17:
	case 0xFF19:
	case 0xFF1A:
	case 0xFF1C:
	case 0xFF1E:
	case 0xFF1F:
	case 0xFF21:
	case 0xFF23:
	case 0xFF24:
	case 0xFF25:
	case 0xFF26: return m_audioMem[address - 0xFF10]; break;
	case 0xFF40: return m_ppu->getControl(); break;
	case 0xFF41: m_ppu->getLcdStatus(); break;
	case 0xFF42: return m_ppu->getScy(); break;
	case 0xFF43: return m_ppu->getScx(); break;
	case 0xFF44: return m_ppu->getLy(); break;
	case 0xFF45: return m_ppu->getLyc(); break;
	case 0xFF46: throw std::runtime_error("Bus: Unhandled read to 0xFF46. OAM DMA transfer not implemented."); break;
	case 0xFF47: return m_ppu->getPalette(); break;
	case 0xFF48: return m_ppu->getObjPalette0(); break;
	case 0xFF49: return m_ppu->getObjPalette1(); break;
	case 0xFF4A: return m_ppu->getWy(); break;
	case 0xFF4B: return m_ppu->getWx(); break;
	case 0xFF50: return m_cartridge->isBootRomMapped(); break;
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
			   << uint(address) << std::endl;
		throw std::runtime_error(stream.str());
	}
	}
}

void Bus::ioWrite8(const uint16_t address, const uint8_t value) {
	switch(address) {
	case 0xff01:
	case 0xff02: /* Ignore serial transfers */ break;
	case 0xFF0F: m_cpu->setInterruptFlagRaw(value); break;
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
	case 0xFF41: m_ppu->setLcdStatus(value); break;
	case 0xFF42: m_ppu->setScy(value); break;
	case 0xFF43: m_ppu->setScx(value); break;
	case 0xFF45: m_ppu->setLyc(value); break;
	case 0xFF46: throw std::runtime_error("Bus: Unhandled write to 0xFF46. OAM DMA transfer not implemented."); break;
	case 0xFF47: m_ppu->setPalette(value); break;
	case 0xFF48: m_ppu->setObjPalette0(value); break;
	case 0xFF49: m_ppu->setObjPalette1(value); break;
	case 0xFF4A: m_ppu->setWy(value); break;
	case 0xFF4B: m_ppu->setWx(value); break;
	case 0xFF50:
		m_cpu->initializeRegisters();
		m_cartridge->unmapBootRom();
		break;
	case 0xFF7F:
		/* Tetris bug. Ignore write. */
		break;
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
			   << uint(address) << std::endl;
		throw std::runtime_error(stream.str());
	}
	}
}
