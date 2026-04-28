#include "bus.hpp"
#include "cartridge/cartridge.hpp"
#include "config.hpp"
#include "cpu/cpu.hpp"
#include "common.hpp"
#include "graphics/ppu.hpp"
#include <cstdint>
#include <immintrin.h>
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
		return m_cpu->getInterruptEnableRaw(); // Is this readable?
	} else if(IS_FORBIDDEN(address)) {
		std::cout << "Bus: Ignoring read on forbidden address 0x" << std::hex << std::setw(4) << std::setfill('0')
				  << uint(address) << std::endl;
		return 0xFF;
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
		m_cpu->setInterruptEnableRaw(value);
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
	/* Joypad */
	case 0xFF00: return m_joypad->read8();

	/* Serial transfers */
	case 0xFF01:
	case 0xFF02: return 0;

	/* Unused I/O */
	case 0xFF03: return 0xFF;

	/* Timer */
	case 0xFF04: return m_timer->getDiv();
	case 0xFF05: return m_timer->getTima();
	case 0xFF06: return m_timer->getTma();
	case 0xFF07: return m_timer->getTac();

	/* Unused I/O */
	case 0xFF08:
	case 0xFF09:
	case 0xFF0A:
	case 0xFF0B:
	case 0xFF0C:
	case 0xFF0D:
	case 0xFF0E: return 0xFF;

	/* IE */
	case 0xFF0F: return m_cpu->getInterruptFlagRaw();

	/* Sound */
	case 0xFF10:
	case 0xFF11:
	case 0xFF12:
	case 0xFF13:
	case 0xFF14: return m_apu->read8(address);

	/* Unused I/O */
	case 0xFF15: return 0xFF;

	/* Sound */
	case 0xFF16:
	case 0xFF17:
	case 0xFF18:
	case 0xFF19: return m_apu->read8(address);

	case 0xFF1A:
	case 0xFF1B:
	case 0xFF1C:
	case 0xFF1D:
	case 0xFF1E: return m_apu->read8(address);

	/* Unused I/O */
	case 0xFF1F: return 0xFF;

	/* Sound */
	case 0xFF20:
	case 0xFF21:
	case 0xFF22:
	case 0xFF23: return m_apu->read8(address);
	case 0xFF24:
	case 0xFF25: return 0xFF;
	case 0xFF26: return m_apu->read8(address);

	/* Unused I/O */
	case 0xFF27:
	case 0xFF28:
	case 0xFF29:
	case 0xFF2A:
	case 0xFF2B:
	case 0xFF2C:
	case 0xFF2D:
	case 0xFF2E:
	case 0xFF2F: return 0xFF;

	/* Sound */
	case 0xFF30:
	case 0xFF31:
	case 0xFF32:
	case 0xFF33:
	case 0xFF34:
	case 0xFF35:
	case 0xFF36:
	case 0xFF37:
	case 0xFF38:
	case 0xFF39:
	case 0xFF3A:
	case 0xFF3B:
	case 0xFF3C:
	case 0xFF3D:
	case 0xFF3E:
	case 0xFF3F: return m_apu->read8(address);

	/* PPU & LCD */
	case 0xFF40: return m_ppu->getControl();
	case 0xFF41: return m_ppu->getLcdStatus();
	case 0xFF42: return m_ppu->getScy();
	case 0xFF43: return m_ppu->getScx();
	case 0xFF44: return m_ppu->getLy();
	case 0xFF45: return m_ppu->getLyc();
	case 0xFF46: return m_oamSourceAndStart;
	case 0xFF47: return m_ppu->getPalette();
	case 0xFF48: return m_ppu->getObjPalette0();
	case 0xFF49: return m_ppu->getObjPalette1();
	case 0xFF4A: return m_ppu->getWy();
	case 0xFF4B: return m_ppu->getWx();

	/* CGB only */
	case 0xFF4C:
	case 0xFF4D: return 0xFF;

	/* Unused I/O */
	case 0xFF4E: return 0xFF;

	/* CGB only */
	case 0xFF4F: return 0xFF;

	/* Cartridge & boot ROM */
	case 0xFF50: return m_cartridge->isBootRomMapped();

	/* CGB only */
	case 0xFF51:
	case 0xFF52:
	case 0xFF53:
	case 0xFF54:
	case 0xFF55:
	case 0xFF56: return 0xFF;

	/* Unused I/O */
	case 0xFF57:
	case 0xFF58:
	case 0xFF59:
	case 0xFF5A:
	case 0xFF5B:
	case 0xFF5C:
	case 0xFF5D:
	case 0xFF5E:
	case 0xFF5F:
	case 0xFF60:
	case 0xFF61:
	case 0xFF62:
	case 0xFF63:
	case 0xFF64:
	case 0xFF65:
	case 0xFF66:
	case 0xFF67: return 0xFF;

	/* CGB only */
	case 0xFF68:
	case 0xFF69:
	case 0xFF6A:
	case 0xFF6B:
	case 0xFF6C: return 0xFF;

	/* Unused I/O */
	case 0xFF6D:
	case 0xFF6E:
	case 0xFF6F: return 0xFF;

	/* CGB only */
	case 0xFF70: return 0xFF;

	/* Unused I/O */
	case 0xFF71:
	case 0xFF72:
	case 0xFF73:
	case 0xFF74:
	case 0xFF75: return 0xFF;

	/* CGB only */
	case 0xFF76:
	case 0xFF77: return 0xFF;

	/* Unused I/O */
	case 0xFF78:
	case 0xFF79:
	case 0xFF7A:
	case 0xFF7B:
	case 0xFF7C:
	case 0xFF7D:
	case 0xFF7E:
	case 0xFF7F: return 0xFF;
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
	/* Joypad */
	case 0xFF00: m_joypad->write8(value); break;

	/* Serial transfers */
	case 0xFF01:
		if(Config::serialOutput) { std::cout << uint8_t(value); }
		break;
	case 0xFF02: break;

	/* Unused I/O */
	case 0xFF03: break;

	/* Timer */
	case 0xFF04: m_timer->resetDiv(); break;
	case 0xFF05: m_timer->setTima(value); break;
	case 0xFF06: m_timer->setTma(value); break;
	case 0xFF07: m_timer->setTac(value); break;

	/* Unused I/O */
	case 0xFF08:
	case 0xFF09:
	case 0xFF0A:
	case 0xFF0B:
	case 0xFF0C:
	case 0xFF0D:
	case 0xFF0E: break;

	/* IE */
	case 0xFF0F: m_cpu->setInterruptFlagRaw(value); break;

	/* Sound */
	case 0xFF10:
	case 0xFF11:
	case 0xFF12:
	case 0xFF13:
	case 0xFF14: m_apu->write8(address, value); break;

	/* Unused I/O */
	case 0xFF15: break;

	/* Sound */
	case 0xFF16:
	case 0xFF17:
	case 0xFF18:
	case 0xFF19: m_apu->write8(address, value); break;

	/* Sound */
	case 0xFF1A:
	case 0xFF1B:
	case 0xFF1C:
	case 0xFF1D:
	case 0xFF1E: m_apu->write8(address, value); break;

	/* Unused I/O */
	case 0xFF1F: break;

	/* Sound */
	case 0xFF20:
	case 0xFF21:
	case 0xFF22:
	case 0xFF23: m_apu->write8(address, value); break;
	case 0xFF24:
	case 0xFF25: break;
	case 0xFF26: m_apu->write8(address, value); break;

	/* Unused I/O */
	case 0xFF27:
	case 0xFF28:
	case 0xFF29:
	case 0xFF2A:
	case 0xFF2B:
	case 0xFF2C:
	case 0xFF2D:
	case 0xFF2E:
	case 0xFF2F: break;

	/* Sound */
	case 0xFF30:
	case 0xFF31:
	case 0xFF32:
	case 0xFF33:
	case 0xFF34:
	case 0xFF35:
	case 0xFF36:
	case 0xFF37:
	case 0xFF38:
	case 0xFF39:
	case 0xFF3A:
	case 0xFF3B:
	case 0xFF3C:
	case 0xFF3D:
	case 0xFF3E:
	case 0xFF3F: m_apu->write8(address, value); break;

	/* PPU & LCD */
	case 0xFF40: m_ppu->setControl(value); break;
	case 0xFF41: m_ppu->setLcdStatus(value); break;
	case 0xFF42: m_ppu->setScy(value); break;
	case 0xFF43: m_ppu->setScx(value); break;
	case 0xFF44: break;
	case 0xFF45: m_ppu->setLyc(value); break;
	case 0xFF46:
		m_oamSourceAndStart = value;
		doDmaTransfer();
		break;
	case 0xFF47: m_ppu->setPalette(value); break;
	case 0xFF48: m_ppu->setObjPalette0(value); break;
	case 0xFF49: m_ppu->setObjPalette1(value); break;
	case 0xFF4A: m_ppu->setWy(value); break;
	case 0xFF4B: m_ppu->setWx(value); break;

	/* CGB only */
	case 0xFF4C:
	case 0xFF4D: break;

	/* Unused I/O */
	case 0xFF4E: break;

	/* CGB only */
	case 0xFF4F: break;

	/* Cartridge & boot ROM */
	case 0xFF50:
		if(!m_cartridge->isBootRomMapped()) {
			std::cout << "Cartridge: Warning, boot ROM already unmapped. Ignoring 0xFF50 write." << std::endl;
		} else {
			m_introEnded = true;
			m_cpu->initializeRegisters();
			m_cartridge->unmapBootRom();
		}
		break;

	/* CGB only */
	case 0xFF51:
	case 0xFF52:
	case 0xFF53:
	case 0xFF54:
	case 0xFF55:
	case 0xFF56: break;

	/* Unused I/O */
	case 0xFF57:
	case 0xFF58:
	case 0xFF59:
	case 0xFF5A:
	case 0xFF5B:
	case 0xFF5C:
	case 0xFF5D:
	case 0xFF5E:
	case 0xFF5F:
	case 0xFF60:
	case 0xFF61:
	case 0xFF62:
	case 0xFF63:
	case 0xFF64:
	case 0xFF65:
	case 0xFF66:
	case 0xFF67: break;

	/* CGB only */
	case 0xFF68:
	case 0xFF69:
	case 0xFF6A:
	case 0xFF6B:
	case 0xFF6C: break;

	/* Unused I/O */
	case 0xFF6D:
	case 0xFF6E:
	case 0xFF6F: break;

	/* CGB only */
	case 0xFF70: break;

	/* Unused I/O */
	case 0xFF71:
	case 0xFF72:
	case 0xFF73:
	case 0xFF74:
	case 0xFF75: break;

	/* CGB only */
	case 0xFF76:
	case 0xFF77: break;

	/* Unused I/O */
	case 0xFF78:
	case 0xFF79:
	case 0xFF7A:
	case 0xFF7B:
	case 0xFF7C:
	case 0xFF7D:
	case 0xFF7E:
	case 0xFF7F: break;
	default: {
		std::stringstream stream;
		stream << "Bus: Illegal I/O write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
			   << uint(address) << std::endl;
		throw std::runtime_error(stream.str());
	}
	}
}

void Bus::requestInterrupt(InterruptRequestType interruptType) {
	switch(interruptType) {
	case InterruptRequestType::Joypad: m_cpu->setInterruptFlag<Cpu::InterruptFlag::Joypad>(true); break;
	case InterruptRequestType::Serial:
		m_cpu->setInterruptFlag<Cpu::InterruptFlag::Serial>(true);
		break;
		break;
	case InterruptRequestType::Timer:
		m_cpu->setInterruptFlag<Cpu::InterruptFlag::Timer>(true);
		break;
		break;
	case InterruptRequestType::Lcd:
		m_cpu->setInterruptFlag<Cpu::InterruptFlag::Lcd>(true);
		break;
		break;
	case InterruptRequestType::VBlank:
		m_cpu->setInterruptFlag<Cpu::InterruptFlag::VBlank>(true);
		break;
		break;
	}
}


void Bus::doDmaTransfer(void) {
	for(uint8_t offset = 0; offset <= 0x9F; offset++) {
		const uint16_t source = (static_cast<uint16_t>(m_oamSourceAndStart) << 8) | offset;
		const uint16_t destination = 0xFE00 | offset;

		const uint8_t value = read8(source);
		write8(destination, value);
	}
}
