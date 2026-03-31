#pragma once

#include "cartridge.hpp"
#include "common.hpp"
#include <array>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

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
				return BOOT_ROM[address];
			} else {
				return m_rom[address];
			}
		} else if(IN_RANGE(address, 0xA000, 0xBFFF)) {
			return m_ram[address - 0xA000];
		} else {
			std::stringstream stream;
			stream << "Cartridge: Illegal read on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << int(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
	}

	void write8(const uint16_t address, const uint8_t value) override {
		if(address <= 0x7FFF) {
			if(isBootRomMapped() && address < 0x100) {
				std::cout << "m_bootRomMapped=" << std::boolalpha << isBootRomMapped() << std::endl;
				std::stringstream stream;
				stream << "Cartridge: Illegal write to boot ROM on address 0x" << std::hex << std::setw(4)
					   << std::setfill('0') << int(address) << std::endl;
				throw std::runtime_error(stream.str());
			} else {
				m_rom[address] = value;
			}
		} else if(IN_RANGE(address, 0xA000, 0xBFFF)) {
			m_ram[address - 0xA000] = value;
		} else {
			std::stringstream stream;
			stream << "Cartridge: Illegal write on address 0x" << std::hex << std::setw(4) << std::setfill('0')
				   << uint(address) << std::endl;
			throw std::runtime_error(stream.str());
		}
	}

	const char *data() const override { return m_rom; }

  private:
	char m_rom[CARTRIDGE_ROM_SIZE];
	char m_ram[CARTRIDGE_RAM_SIZE];
};
