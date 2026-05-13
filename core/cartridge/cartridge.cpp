#include "cartridge.hpp"
#include "cartridge/no_mbc.hpp"
#include "cartridge/mbc1.hpp"
#include <fstream>
#include <iomanip>
#include <ios>
#include <sstream>
#include <stdexcept>

std::unique_ptr<Cartridge> Cartridge::createCartridge(const std::string &path) {
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
	case 0x00: return std::make_unique<NoMBC>(fileData); break;
	case 0x01:
	case 0x02:
	case 0x03: return std::make_unique<MBC1>(fileData); break;
	default:
		std::stringstream stream;
		stream << "Cartridge: Unimplemented cartridge of type \""
			   << (CARTRIDGE_TYPES.find(type) != CARTRIDGE_TYPES.end() ? CARTRIDGE_TYPES.at(type) : "Unknown")
			   << "\" (0x" << std::hex << std::setw(2) << std::setfill('0') << uint(type) << ")" << std::endl;
		throw std::runtime_error(stream.str());
	}
}
