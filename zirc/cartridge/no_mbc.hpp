#pragma once

#include "cartridge.hpp"
#include "../common.hpp"
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace Zirc {
#define CARTRIDGE_ROM_SIZE (1 << 15)
#define CARTRIDGE_RAM_SIZE (1 << 13)

class NoMBC : public Cartridge {
  public:
	NoMBC(const std::vector<uint8_t> cartridgeData) : Cartridge(cartridgeData) {
		assert(type() == 0x00);
		std::memcpy(m_rom, cartridgeData.data(), sizeof(uint8_t) * cartridgeData.size());
	}

	uint8_t read8(const uint16_t address) const override {
		if(address <= 0x7FFF) {
			if(isBootRomMapped() && address < 0x100) {
				return read8BootRom(address);
			} else {
				return m_rom[address];
			}
		} else if(IN_RANGE(address, 0xA000, 0xBFFF)) {
			return m_ram[address - 0xA000];
		} else {
			std::stringstream stream;
			stream << "Cartridge[NoMBC]: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << int(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
	}

	void write8(const uint16_t address, const uint8_t value) override {
		if(address <= 0x7FFF) {
			std::cout << "Cartridge[NoMBC]: Ignoring write to Cartridge's ROM at address 0x" << std::hex << std::setw(4)
					  << std::setfill('0') << address << " with value 0x" << std::setw(2) << value << std::endl;
			return;
		} else if(IN_RANGE(address, 0xA000, 0xBFFF)) {
			m_ram[address - 0xA000] = value;
		} else {
			std::stringstream stream;
			stream << "Cartridge[NoMBC]: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
	}

	void reset() override {
		Cartridge::reset();

		std::memset(m_ram, 0, sizeof(m_ram));
	}

  private:
	uint8_t m_rom[CARTRIDGE_ROM_SIZE];
	uint8_t m_ram[CARTRIDGE_RAM_SIZE];
};
} // namespace Zirc
