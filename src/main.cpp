#include "bus.hpp"
#include "cpu.hpp"
#include "gameboy.hpp"
#include "memory.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char *argv[]) {
	if(argc != 2) {
		std::cout << "Usage:" << std::endl;
		std::cout << "\t" << argv[0] << " <ROM path>" << std::endl;
		return 1;
	}

	GameBoy gb(argv[1]);
	gb.debugCartridge();
	std::cout << "Press any key to start the Game Boy..." << std::endl;
	std::cin.get();
	gb.start();

	return 0;
}
