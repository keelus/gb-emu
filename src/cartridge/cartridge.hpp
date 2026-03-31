#pragma once

#include <array>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#define CARTRIDGE_NINTENDO_LOGO_OFFSET 0x104
#define CARTRIDGE_NINTENDO_LOGO_LEN 48

#define CARTRIDGE_TITLE_OFFSET 0x134
#define CARTRIDGE_TITLE_LEN 16

#define CARTRIDGE_MANUFRACTURER_CODE_OFFSET 0x13F
#define CARTRIDGE_MANUFRACTURER_CODE_LEN 4

#define CARTRIDGE_CGB_FLAG_OFFSET 0x143

#define CARTRIDGE_NEW_LICENSEE_CODE_OFFSET 0x144
#define CARTRIDGE_NEW_LICENSEE_CODE_LEN 2

#define CARTRIDGE_SGB_FLAG_OFFSET 0x146

#define CARTRIDGE_TYPE_OFFSET 0x147

const std::unordered_map<uint8_t, std::string_view> CARTRIDGE_TYPES = {
	{0x00,					   "ROM ONLY"},
	{0x01,						   "MBC1"},
	{0x02,					   "MBC1+RAM"},
	{0x03,			   "MBC1+RAM+BATTERY"},
	{0x05,						   "MBC2"},
	{0x06,				   "MBC2+BATTERY"},
	{0x08,					 "ROM+RAM 11"},
	{0x09,			 "ROM+RAM+BATTERY 11"},
	{0x0B,						  "MMM01"},
	{0x0C,					  "MMM01+RAM"},
	{0x0D,			  "MMM01+RAM+BATTERY"},
	{0x0F,			 "MBC3+TIMER+BATTERY"},
	{0x10,	   "MBC3+TIMER+RAM+BATTERY 12"},
	{0x11,						   "MBC3"},
	{0x12,					"MBC3+RAM 12"},
	{0x13,			"MBC3+RAM+BATTERY 12"},
	{0x19,						   "MBC5"},
	{0x1A,					   "MBC5+RAM"},
	{0x1B,			   "MBC5+RAM+BATTERY"},
	{0x1C,					"MBC5+RUMBLE"},
	{0x1D,				"MBC5+RUMBLE+RAM"},
	{0x1E,		   "MBC5+RUMBLE+RAM+BATTERY"},
	{0x20,						   "MBC6"},
	{0x22, "MBC7+SENSOR+RUMBLE+RAM+BATTERY"},
	{0xFC,				  "POCKET CAMERA"},
	{0xFD,				   "BANDAI TAMA5"},
	{0xFE,						   "HuC3"},
	{0xFF,			   "HuC1+RAM+BATTERY"}
};

#define CARTRIDGE_ROM_TYPE_OFFSET 0x148

const std::unordered_map<uint8_t, std::string_view> ROM_SIZES = {
	{0x00, "32 KiB [2 ROM banks (no-baking)]"},
	{0x01,			 "64 KiB [4 ROM banks]"},
	{0x02,			"128 KiB [8 ROM banks]"},
	{0x03,		   "256 KiB [16 ROM banks]"},
	{0x04,		   "512 KiB [32 ROM banks]"},
	{0x05,			 "1 MiB [64 ROM banks]"},
	{0x06,			"2 MiB [128 ROM banks]"},
	{0x07,			"4 MiB [256 ROM banks]"},
	{0x08,			"8 MiB [512 ROM banks]"},
	{0x52,			   "1.1 MiB [72 banks]"},
	{0x53,			   "1.2 MiB [80 banks]"},
	{0x54,			   "1.5 MiB [96 banks]"}
};

#define CARTRIDGE_RAM_TYPE_OFFSET 0x149

const std::unordered_map<uint8_t, std::string_view> RAM_SIZES = {
	{0x00,		   "0 [No RAM]"},
	{0x01,		   "– Unused"},
	  {0x02,	 "8 KiB [1 bank]"},
	{0x03,   "32 KiB [4 banks]"},
	{0x04, "128 KiB [16 banks]"},
	  {0x05,	 "64 KiB [8 banks]"}
};

#define CARTRIDGE_DESTINATION_CODE_OFFSET 0x14A

#define CARTRIDGE_OLD_LICENSEE_CODE_OFFSET 0x14B

#define CARTRIDGE_ROM_VERSION_OFFSET 0x14C

#define CARTRIDGE_HEADER_CHECKSUM_OFFSET 0x14D

// clang-format off
const std::array<uint8_t, 256> BOOT_ROM = {
    0x31, 0xFE, 0xFF, 0xAF, 0x21, 0xFF, 0x9F, 0x32, 0xCB, 0x7C, 0x20, 0xFB, 0x21, 0x26, 0xFF, 0x0E,
    0x11, 0x3E, 0x80, 0x00, 0x00, 0x0C, 0x3E, 0xF3, 0x00, 0x00, 0x3E, 0x77, 0x00, 0x3E, 0xFC, 0xE0,
    0x47, 0x11, 0xA8, 0x00, 0x21, 0x10, 0x80, 0x1A, 0xCD, 0x95, 0x00, 0xCD, 0x96, 0x00, 0x13, 0x7B,
    0xFE, 0x34, 0x20, 0xF3, 0x11, 0xD8, 0x00, 0x06, 0x08, 0x1A, 0x13, 0x22, 0x23, 0x05, 0x20, 0xF9,
    0x3E, 0x19, 0xEA, 0x10, 0x99, 0x21, 0x2F, 0x99, 0x0E, 0x0C, 0x3D, 0x28, 0x08, 0x32, 0x0D, 0x20,
    0xF9, 0x2E, 0x0F, 0x18, 0xF3, 0x67, 0x3E, 0x64, 0x57, 0xE0, 0x42, 0x3E, 0x91, 0xE0, 0x40, 0x04,
    0x1E, 0x02, 0x0E, 0x0C, 0xF0, 0x44, 0xFE, 0x90, 0x20, 0xFA, 0x0D, 0x20, 0xF7, 0x1D, 0x20, 0xF2,
    0x0E, 0x13, 0x24, 0x7C, 0x1E, 0x83, 0xFE, 0x62, 0x28, 0x06, 0x1E, 0xC1, 0xFE, 0x64, 0x20, 0x06,
    0x7B, 0x00, 0x0C, 0x3E, 0x87, 0x00, 0xF0, 0x42, 0x90, 0xE0, 0x42, 0x15, 0x20, 0xD2, 0x05, 0x20,
    0x4F, 0x16, 0x20, 0x18, 0xCB, 0x4F, 0x06, 0x04, 0xC5, 0xCB, 0x11, 0x17, 0xC1, 0xCB, 0x11, 0x17,
    0x05, 0x20, 0xF5, 0x22, 0x23, 0x22, 0x23, 0xC9, 0x13, 0x33, 0xF1, 0x10, 0x3B, 0xB3, 0x00, 0x0E,
    0x00, 0x00, 0x00, 0x01, 0x00, 0x0E, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x0C, 0x00, 0x0C, 0x00, 0x0C,
    0x33, 0x31, 0x31, 0x1F, 0xBB, 0xB3, 0x33, 0x3E, 0x70, 0x00, 0xB3, 0x31, 0x3F, 0x0F, 0x76, 0x66,
    0x76, 0x66, 0x66, 0x66, 0xCC, 0xD6, 0xCC, 0xCC, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x21, 0xA8, 0x00, 0x11, 0xA8, 0x00, 0x1A, 0x13, 0xBE, 0x20, 0xFE, 0x23, 0x7D, 0xFE, 0x34, 0x00,
    0x00, 0x06, 0x19, 0x78, 0x86, 0x23, 0x05, 0x20, 0xFB, 0x86, 0x00, 0x00, 0x3E, 0x01, 0xE0, 0x50
};
// clang-format on

class Cartridge {
  public:
	Cartridge(const std::vector<char> fileData) {
		m_title = std::string(fileData.data() + CARTRIDGE_TITLE_OFFSET, CARTRIDGE_TITLE_LEN);

		m_manufacturerCode = std::vector<uint8_t>(fileData.data() + CARTRIDGE_MANUFRACTURER_CODE_OFFSET,
												  fileData.data() + CARTRIDGE_MANUFRACTURER_CODE_OFFSET +
													  CARTRIDGE_MANUFRACTURER_CODE_LEN);

		m_cgbFlag = fileData[CARTRIDGE_CGB_FLAG_OFFSET];

		m_newLicenseeCode =
			(static_cast<uint16_t>(static_cast<uint8_t>(fileData[CARTRIDGE_NEW_LICENSEE_CODE_OFFSET])) << 8) |
			static_cast<uint16_t>(static_cast<uint8_t>(fileData[CARTRIDGE_NEW_LICENSEE_CODE_OFFSET + 1]));

		m_sgbFlag = fileData[CARTRIDGE_SGB_FLAG_OFFSET];
		m_type = fileData[CARTRIDGE_TYPE_OFFSET];
		m_romType = fileData[CARTRIDGE_ROM_TYPE_OFFSET];
		m_ramType = fileData[CARTRIDGE_RAM_TYPE_OFFSET];
		m_destinationCode = fileData[CARTRIDGE_DESTINATION_CODE_OFFSET];
		m_oldLicenseeCode = fileData[CARTRIDGE_OLD_LICENSEE_CODE_OFFSET];
		m_romVersionNumber = fileData[CARTRIDGE_ROM_VERSION_OFFSET];
		m_headerChecksum = fileData[CARTRIDGE_HEADER_CHECKSUM_OFFSET];

		m_bootRomMapped = true;
	}

	static std::unique_ptr<Cartridge> createCartridge(const std::string &path);

	void debug(void) const {
		std::cout << "== ROM DEBUG ==" << std::endl;
		std::cout << "- Title: \"" << title() << "\"" << std::endl;
		std::cout << "- Type: "
				  << (CARTRIDGE_TYPES.find(type()) != CARTRIDGE_TYPES.end() ? CARTRIDGE_TYPES.at(type()) : "Unknown")
				  << " (0x" << std::hex << std::setw(2) << std::setfill('0') << type() << ")" << std::endl;
		std::cout << "- ROM size: "
				  << (ROM_SIZES.find(romType()) != ROM_SIZES.end() ? ROM_SIZES.at(romType()) : "Unknown") << " (0x"
				  << std::hex << std::setw(2) << std::setfill('0') << romType() << ")" << std::endl;
		std::cout << "- RAM size: "
				  << (RAM_SIZES.find(ramType()) != RAM_SIZES.end() ? RAM_SIZES.at(ramType()) : "Unknown") << " (0x"
				  << std::hex << std::setw(2) << std::setfill('0') << ramType() << ")" << std::endl;
		std::cout << "- ROM version: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(romVersion())
				  << std::endl;
		std::cout << "- Destination code: 0x" << std::hex << std::setw(2) << std::setfill('0') << int(destinationCode())
				  << std::endl;
		std::cout << "- Licensee code (" << (oldLicenseeCode() == 0x33 ? "new" : "old") << "): 0x" << std::hex
				  << std::setw(2) << std::setfill('0') << int(licenseeCode()) << std::endl;
		std::cout << "- Header checksum (" << (calculateHeaderChecksum() == headerChecksum() ? "correct" : "incorrect")
				  << "): 0x" << std::hex << std::setw(2) << std::setfill('0') << int(headerChecksum()) << std::endl;
	}

	std::string title() const { return m_title; }
	uint8_t type() const { return m_type; }

	uint16_t licenseeCode(void) const {
		uint8_t old = oldLicenseeCode();
		if(old == 0x33) { return old; }
		return newLicenseeCode();
	}
	uint16_t newLicenseeCode() const { return m_newLicenseeCode; }
	uint8_t oldLicenseeCode() const { return m_oldLicenseeCode; }

	uint8_t flagSGB() const { return m_sgbFlag; }
	uint8_t flagCGB() const { return m_cgbFlag; }

	uint8_t romType() const { return m_romType; }
	uint8_t ramType() const { return m_ramType; }

	std::vector<uint8_t> manufracturerCode() const { return m_manufacturerCode; }

	uint8_t romVersion() const { return m_romVersionNumber; }
	uint8_t destinationCode() const { return m_destinationCode; }

	uint8_t headerChecksum() const { return m_headerChecksum; }

	virtual uint8_t calculateHeaderChecksum(void) const = 0;

	virtual uint8_t read8(const uint16_t address) const = 0;
	virtual void write8(const uint16_t address, const uint8_t value) = 0;

	void unmapBootRom() { m_bootRomMapped = false; }
	bool isBootRomMapped() const { return m_bootRomMapped; }

	virtual const char *data() const = 0;

  private:
	std::string m_title;
	std::vector<uint8_t> m_manufacturerCode;
	bool m_cgbFlag;
	uint16_t m_newLicenseeCode;
	bool m_sgbFlag;
	uint8_t m_type;
	uint8_t m_romType;
	uint8_t m_ramType;
	uint8_t m_destinationCode;
	uint8_t m_oldLicenseeCode;
	uint8_t m_romVersionNumber;
	uint8_t m_headerChecksum;

	bool m_bootRomMapped;
};
