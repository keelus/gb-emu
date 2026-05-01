#include "emulator.hpp"
#include <cassert>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include "config.hpp"
#include "gameboy.hpp"
#include "platform.hpp"

uint8_t activeColorPalette = 0;

void Emulator::printUsage(const char *argv0, const bool isHelpMessage) {
	std::cout << "Usage:" << argv0 << " [options] <ROM path>" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t -s, --serial\t\tEnable serial output" << std::endl;
	std::cout << "\t -f, --no-fps\t\tDisable 59.70fps limit" << std::endl;
	std::cout << "\t -d, --debug\t\tEnable debug output" << std::endl;
	std::cout << "\t -i, --skip-intro\tSkip boot intro" << std::endl;
	std::cout << "\t -h, --help\t\tShow " << (isHelpMessage ? "this" : "the") << " help message and exit" << std::endl;
}

int Emulator::run(int argc, char *argv[]) {
	if(argc < 2) {
		printUsage(argv[0], false);
		return EXIT_FAILURE;
	}

	const char *romPath = NULL;

	for(size_t i = 1; i < argc; i++) {
		const char *arg = argv[i];

		if(!strcmp(arg, "-s") || !strcmp(arg, "--serial")) {
			Config::serialOutput = true;
		} else if(!strcmp(arg, "-f") || !strcmp(arg, "--no-fps")) {
			Config::limitFps = false;
		} else if(!strcmp(arg, "-d") || !strcmp(arg, "--debug")) {
			Config::debugOutput = true;
		} else if(!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			printUsage(argv[0], true);
			return EXIT_SUCCESS;
		} else if(!strcmp(arg, "-i") || !strcmp(arg, "--skip-intro")) {
			Config::skipIntro = true;
		} else {
			romPath = arg;
		}
	}

	if(romPath == NULL) {
		std::cout << "You must provide the path to a valid Game Boy ROM file." << std::endl;
		printUsage(argv[0], false);
		return EXIT_FAILURE;
	}

	GameBoy gb(romPath, m_platform);
	gb.debugCartridge();
	std::cout << "Press any key to start the Game Boy..." << std::endl;
	std::cin.get();

	m_platform.addGameBoy(&gb);

	while(Config::skipIntro && !gb.introEnded()) {
		gb.tick();
	}

	while(m_platform.running()) {
		m_platform.beforeFrame();

		int cycles = 0;
		while(cycles < GameBoy::CYCLES_PER_FRAME) {
			cycles += gb.tick();
		}

		m_platform.showFrame();
		m_platform.afterFrame();
	}

	return EXIT_SUCCESS;
}
