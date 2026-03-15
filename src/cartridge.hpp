#pragma once

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include <vector>

#define CARTRIDGE_ROM_SIZE (1 << 15)
#define CARTRIDGE_RAM_SIZE (1 << 13)

#define CARTRIDGE_ENTRY_OFFSET 0x100
#define CARTRIDGE_ENTRY_LEN 4

#define CARTRIDGE_NINTENDO_LOGO_OFFSET 0x104
#define CARTRIDGE_NINTENDO_LOGO_LEN 48

#define CARTRIDGE_TITLE_OFFSET 0x134
#define CARTRIDGE_TITLE_LEN 16

#define CARTRIDGE_MANUFRACTURER_CODE_OFFSET 0x13F
#define CARTRIDGE_MANUFRACTURER_CODE_LEN 4

#define CARTRIDGE_CGB_FLAG_OFFSET 0x143
#define CARTRIDGE_CGB_FLAG_LEN 1

#define CARTRIDGE_NEW_LICENSEE_CODE_OFFSET 0x144
#define CARTRIDGE_NEW_LICENSEE_CODE_LEN 2

#define CARTRIDGE_SGB_FLAG_OFFSET 0x146
#define CARTRIDGE_SGB_FLAG_LEN 1

#define CARTRIDGE_TYPE_OFFSET 0x147
#define CARTRIDGE_TYPE_LEN 1

#define CARTRIDGE_ROM_SIZE_OFFSET 0x148
#define CARTRIDGE_ROM_SIZE_LEN 1

constexpr std::array<const char *, 5> ROM_SIZE_DEBUG = {
	"32 KiB (2 ROM banks)",	  "64 KiB (4 ROM banks)",	"128 KiB (8 ROM banks)",
	"256 KiB (16 ROM banks)", "512 KiB (32 ROM banks)",
};

#define CARTRIDGE_RAM_SIZE_OFFSET 0x149
#define CARTRIDGE_RAM_SIZE_LEN 1

constexpr std::array<const char *, 6> RAM_SIZE_DEBUG = {
	"0 (no RAM)", "-", "8 KiB (1 bank)", "32 KiB (4 banks)", "128 KiB (16 banks)", "64 KiB (8 banks)",
};

#define CARTRIDGE_DESTINATION_CODE_OFFSET 0x14A
#define CARTRIDGE_DESTINATION_CODE_LEN 1

#define CARTRIDGE_OLD_LICENSEE_CODE_OFFSET 0x14B
#define CARTRIDGE_OLD_LICENSEE_CODE_LEN 1

#define CARTRIDGE_ROM_VERSION_OFFSET 0x14C
#define CARTRIDGE_ROM_VERSION_LEN 1

#define CARTRIDGE_HEADER_CHECKSUM_OFFSET 0x14D
#define CARTRIDGE_HEADER_CHECKSUM_LEN 1

// clang-format off
const std::array<uint8_t, 256> BOOT_ROM = {/* TODO: Upload the BOOT ROM OR handle from file */};
// clang-format on

class Cartridge {
  public:
	Cartridge(const std::string &path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if(!file) { throw std::runtime_error("Could not open the file.\n"); }

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if(size > CARTRIDGE_ROM_SIZE) {
			throw std::runtime_error("The read cartridge was " + std::to_string(size) +
									 " bytes long, but cartridge is only " + std::to_string(CARTRIDGE_ROM_SIZE) +
									 " bytes long.");
		}

		if(!file.read(m_rom, size)) { throw std::runtime_error("Failed to read file.\n"); }
	}

	Cartridge(const char *rom, const size_t rom_len) {
		if(rom_len - 0x1 >= (CARTRIDGE_ROM_SIZE - 0x100)) {
			throw std::runtime_error(
				"The read cartridge was " + std::to_string(rom_len) + " bytes long, but cartridge is only " +
				std::to_string(CARTRIDGE_ROM_SIZE - 0x100) + " bytes long (rom is being loaded at 0x100).");
		}

		std::memcpy(m_rom + 0x100, rom, sizeof(uint8_t) * rom_len);
	}

	void debug(void) const {
		std::cout << "== ROM DEBUG ==" << std::endl;
		std::cout << "- Title: \"" << title() << "\"" << std::endl;
		std::cout << "- Type: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(type()) << std::endl;
		std::cout << "- ROM size(code): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(romSize()) << " ["
				  << ROM_SIZE_DEBUG[romSize()] << "]" << std::endl;
		std::cout << "- RAM size(code): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(ramSize()) << " ["
				  << RAM_SIZE_DEBUG[ramSize()] << "]" << std::endl;
		std::cout << "- ROM version: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(romVersion())
				  << std::endl;
		std::cout << "- Destination code: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(destinationCode())
				  << std::endl;
		std::cout << "- Licensee code (" << (oldLicenseeCode() == 0x33 ? "new" : "old") << "): 0x" << std::hex
				  << std::setw(2) << std::setfill('0') << int(licenseeCode()) << std::endl;
		std::cout << "- Header checksum (" << (calculateHeaderChecksum() == headerChecksum() ? "correct" : "incorrect")
				  << "): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(headerChecksum()) << std::endl;
	}

	std::string title() const { return std::string(m_rom + CARTRIDGE_TITLE_OFFSET, CARTRIDGE_TITLE_LEN); }
	uint8_t type() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_TYPE_OFFSET]); }

	uint16_t licenseeCode(void) const {
		uint8_t old = oldLicenseeCode();
		if(old == 0x33) { return old; }
		return newLicenseeCode();
	}
	uint16_t newLicenseeCode() const {
		uint8_t high = static_cast<uint8_t>(m_rom[CARTRIDGE_NEW_LICENSEE_CODE_OFFSET]);
		uint8_t low = static_cast<uint8_t>(m_rom[CARTRIDGE_NEW_LICENSEE_CODE_OFFSET + 1]);

		return (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
	}
	uint8_t oldLicenseeCode() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_OLD_LICENSEE_CODE_OFFSET]); }

	uint8_t flagSGB() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_SGB_FLAG_OFFSET]); }
	uint8_t flagCGB() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_CGB_FLAG_OFFSET]); }

	uint8_t romSize() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_RAM_SIZE_OFFSET]); }
	uint8_t ramSize() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_RAM_SIZE_OFFSET]); }

	std::vector<uint8_t> manufracturerCode() const {
		return std::vector<uint8_t>(m_rom + CARTRIDGE_MANUFRACTURER_CODE_OFFSET,
									m_rom + CARTRIDGE_MANUFRACTURER_CODE_OFFSET + CARTRIDGE_MANUFRACTURER_CODE_LEN);
	}

	uint8_t romVersion() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_ROM_VERSION_OFFSET]); }
	uint8_t destinationCode() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_DESTINATION_CODE_OFFSET]); }

	uint8_t headerChecksum() const { return static_cast<uint8_t>(m_rom[CARTRIDGE_HEADER_CHECKSUM_OFFSET]); }

	uint8_t calculateHeaderChecksum(void) const {
		uint8_t checksum = 0;
		for(uint16_t address = 0x0134; address <= 0x014C; address++) {
			checksum = checksum - m_rom[address] - 1;
		}

		return checksum;
	}

	uint8_t read8(const uint16_t address) const {
		if(address < 0x100) {
			return BOOT_ROM[address];
		} else {
			return m_rom[address];
		}
	}
	void write8(const uint16_t address, const uint8_t value) {
		if(address < 0x100) {
			std::stringstream stream;
			stream << "Cartridge: Illegal write to boot ROM on address 0x" << std::hex << std::setw(4)
				   << std::setfill('0') << int(address) << std::endl;
			throw std::runtime_error(stream.str());

		} else {
			m_rom[address] = value;
		}
	}

	const char *data() const { return m_rom; }

  private:
	char m_rom[CARTRIDGE_ROM_SIZE];
	char m_ram[CARTRIDGE_RAM_SIZE];
};
