#pragma once

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <ios>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#define CATRIDGE_SIZE 0x8000

#define CATRIDGE_ENTRY_OFFSET 0x100
#define CATRIDGE_ENTRY_LEN 4

#define CATRIDGE_NINTENDO_LOGO_OFFSET 0x104
#define CATRIDGE_NINTENDO_LOGO_LEN 48

#define CATRIDGE_TITLE_OFFSET 0x134
#define CATRIDGE_TITLE_LEN 16

#define CATRIDGE_MANUFRACTURER_CODE_OFFSET 0x13F
#define CATRIDGE_MANUFRACTURER_CODE_LEN 4

#define CATRIDGE_CGB_FLAG_OFFSET 0x143
#define CATRIDGE_CGB_FLAG_LEN 1

#define CATRIDGE_NEW_LICENSEE_CODE_OFFSET 0x144
#define CATRIDGE_NEW_LICENSEE_CODE_LEN 2

#define CATRIDGE_SGB_FLAG_OFFSET 0x146
#define CATRIDGE_SGB_FLAG_LEN 1

#define CATRIDGE_TYPE_OFFSET 0x147
#define CATRIDGE_TYPE_LEN 1

#define CATRIDGE_ROM_SIZE_OFFSET 0x148
#define CATRIDGE_ROM_SIZE_LEN 1

#define CATRIDGE_RAM_SIZE_OFFSET 0x149
#define CATRIDGE_RAM_SIZE_LEN 1

#define CATRIDGE_DESTINATION_CODE_OFFSET 0x14A
#define CATRIDGE_DESTINATION_CODE_LEN 1

#define CATRIDGE_OLD_LICENSEE_CODE_OFFSET 0x14B
#define CATRIDGE_OLD_LICENSEE_CODE_LEN 1

#define CATRIDGE_ROM_VERSION_OFFSET 0x14C
#define CATRIDGE_ROM_VERSION_LEN 1

#define CATRIDGE_HEADER_CHECKSUM_OFFSET 0x14D
#define CATRIDGE_HEADER_CHECKSUM_LEN 1

class Catridge {
  public:
	Catridge(const std::string &path) {
		std::ifstream file(path, std::ios::binary | std::ios::ate);
		if(!file) { throw std::runtime_error("Could not open the file.\n"); }

		std::streamsize size = file.tellg();
		file.seekg(0, std::ios::beg);

		if(size > CATRIDGE_SIZE) {
			throw std::runtime_error("The read catridge was " + std::to_string(size) +
									 " bytes long, but catridge is only " + std::to_string(CATRIDGE_SIZE) +
									 " bytes long.");
		}

		if(!file.read(m_data, size)) { throw std::runtime_error("Failed to read file.\n"); }
	}

	void debug(void) const {
		std::cout << "== ROM DEBUG ==" << std::endl;
		std::cout << "- Title: \"" << title() << "\"" << std::endl;
		std::cout << "- Type: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(type()) << std::endl;
		std::cout << "- ROM size(code): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(romSize())
				  << std::endl;
		std::cout << "- RAM size(code): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(ramSize())
				  << std::endl;
		std::cout << "- ROM version: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(romVersion())
				  << std::endl;
		std::cout << "- Destination code: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(destinationCode())
				  << std::endl;
		std::cout << "- Licensee code (" << (oldLicenseeCode() == 0x33 ? "new" : "old") << "): 0x" << std::hex
				  << std::setw(2) << std::setfill('0') << int(licenseeCode()) << std::endl;
		std::cout << "- Header checksum(" << (calculateHeaderChecksum() == headerChecksum() ? "correct" : "incorrect")
				  << "): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(headerChecksum()) << std::endl;
	}

	std::string title() const { return std::string(m_data + CATRIDGE_TITLE_OFFSET, CATRIDGE_TITLE_LEN); }
	uint8_t type() const { return static_cast<uint8_t>(m_data[CATRIDGE_TYPE_OFFSET]); }

	uint16_t licenseeCode(void) const {
		uint8_t old = oldLicenseeCode();
		if(old == 0x33) { return old; }
		return newLicenseeCode();
	}
	uint16_t newLicenseeCode() const {
		uint8_t high = static_cast<uint8_t>(m_data[CATRIDGE_NEW_LICENSEE_CODE_OFFSET]);
		uint8_t low = static_cast<uint8_t>(m_data[CATRIDGE_NEW_LICENSEE_CODE_OFFSET + 1]);

		return (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
	}
	uint8_t oldLicenseeCode() const { return static_cast<uint8_t>(m_data[CATRIDGE_OLD_LICENSEE_CODE_OFFSET]); }

	uint8_t flagSGB() const { return static_cast<uint8_t>(m_data[CATRIDGE_SGB_FLAG_OFFSET]); }
	uint8_t flagCGB() const { return static_cast<uint8_t>(m_data[CATRIDGE_CGB_FLAG_OFFSET]); }

	uint8_t romSize() const { return static_cast<uint8_t>(m_data[CATRIDGE_RAM_SIZE_OFFSET]); }
	uint8_t ramSize() const { return static_cast<uint8_t>(m_data[CATRIDGE_RAM_SIZE_OFFSET]); }


	std::vector<uint8_t> manufracturerCode() const {
		return std::vector<uint8_t>(m_data + CATRIDGE_MANUFRACTURER_CODE_OFFSET,
									m_data + CATRIDGE_MANUFRACTURER_CODE_OFFSET + CATRIDGE_MANUFRACTURER_CODE_LEN);
	}

	uint8_t romVersion() const { return static_cast<uint8_t>(m_data[CATRIDGE_ROM_VERSION_OFFSET]); }
	uint8_t destinationCode() const { return static_cast<uint8_t>(m_data[CATRIDGE_DESTINATION_CODE_OFFSET]); }

	uint8_t headerChecksum() const { return static_cast<uint8_t>(m_data[CATRIDGE_HEADER_CHECKSUM_OFFSET]); }

	uint8_t calculateHeaderChecksum(void) const {
		uint8_t checksum = 0;
		for(uint16_t address = 0x0134; address <= 0x014C; address++) {
			checksum = checksum - m_data[address] - 1;
		}

		return checksum;
	}

	uint8_t read8(const uint16_t address) const { return m_data[address]; }

	void write8(const uint16_t address, const uint8_t value) { m_data[address] = value; }

  private:
	char m_data[0x4000];
};
