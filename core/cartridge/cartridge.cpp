#include "cartridge.hpp"
#include "cartridge/no_mbc.hpp"
#include "cartridge/mbc1.hpp"
#include <fstream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdexcept>

std::unique_ptr<Cartridge> Cartridge::createCartridge(const std::string &path, const char *bootRomPath) {
	bool usingCustomBootRom = false;
	std::array<char, 256> customBootRom;
	if(bootRomPath) {
		usingCustomBootRom = true;
		std::ifstream bootRomFile(bootRomPath, std::ios::binary | std::ios::ate);
		if(!bootRomFile) {
			throw std::runtime_error("Could not open the custom boot ROM file.\n"); // TODO: Should we make this warning
																					// and use default instead?
		}

		std::streamsize bootRomSize = bootRomFile.tellg();
		if(bootRomSize != 256) {
			throw std::runtime_error(
				"The boot ROM's size must be exactly 256 bytes long."); // TODO: Same thing as above
		}

		bootRomFile.seekg(0, std::ios::beg);
		if(!bootRomFile.read(customBootRom.data(), 256)) {
			throw std::runtime_error("Failed to read from the custom boot ROM file.\n");
		}
	}

	std::ifstream file(path, std::ios::binary | std::ios::ate);
	if(!file) { throw std::runtime_error("Could not open the cartridge file.\n"); }

	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	std::vector<uint8_t> fileData(size);

	if(!file.read(reinterpret_cast<char *>(fileData.data()), size)) {
		throw std::runtime_error("Failed to read from the cartridge file.\n");
	}

	uint8_t type = fileData[CARTRIDGE_TYPE_OFFSET];

	switch(type) {
	case 0x00: return std::make_unique<NoMBC>(fileData, usingCustomBootRom ? customBootRom.data() : NULL); break;
	case 0x01:
	case 0x02:
	case 0x03: return std::make_unique<MBC1>(fileData, usingCustomBootRom ? customBootRom.data() : NULL); break;
	default:
		std::stringstream stream;
		stream << "Cartridge: Unimplemented cartridge of type \""
			   << (CARTRIDGE_TYPES.find(type) != CARTRIDGE_TYPES.end() ? CARTRIDGE_TYPES.at(type) : "Unknown")
			   << "\" (0x" << std::hex << std::setw(2) << std::setfill('0') << uint(type) << ")" << std::endl;
		throw std::runtime_error(stream.str());
	}
}
