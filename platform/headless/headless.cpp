#include <csignal>
#include <cstdint>

#include "headless.hpp"
#include <zirc/config.hpp>

uint8_t activeColorPalette = 0;

void printUsage(const char *argv0, const bool isHelpMessage) {
	std::cout << "Usage: " << argv0 << " [options] <ROM path>" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t -b, --boot-rom\t\tPath to a custom boot ROM" << std::endl;
	std::cout << "\t -s, --serial\t\tEnable serial output" << std::endl;
	std::cout << "\t -d, --debug\t\tEnable debug output" << std::endl;
	std::cout << "\t -i, --skip-intro\tSkip boot intro" << std::endl;
	std::cout << "\t -h, --help\t\tShow " << (isHelpMessage ? "this" : "the") << " help message and exit" << std::endl;
}

std::atomic<bool> running = true;
void exitHandler(int signal) {
	if(signal == SIGINT) {
		std::cout << "Exiting..." << std::endl;
		running = false;
	}
}

int main(int argc, char *argv[]) {
	if(argc < 2) {
		printUsage(argv[0], false);
		return EXIT_FAILURE;
	}

	const char *romPath = NULL;

	for(size_t i = 1; i < argc; i++) {
		const char *arg = argv[i];

		if(!strcmp(arg, "-b") || !strcmp(arg, "--boot-rom")) {
			assert(i + 1 < argc && "There was no boot ROM provided after the argument.");
			Config::get().useCustomBootRom = true;
			Config::get().customBootRomPath = argv[++i];
		} else if(!strcmp(arg, "-d") || !strcmp(arg, "--debug")) {
			Config::get().debugOutput = true;
		} else if(!strcmp(arg, "-h") || !strcmp(arg, "--help")) {
			printUsage(argv[0], true);
			return EXIT_SUCCESS;
		} else if(!strcmp(arg, "-i") || !strcmp(arg, "--skip-intro")) {
			Config::get().skipIntro = true;
		} else {
			romPath = arg;
		}
	}

	std::signal(SIGINT, exitHandler);

	if(romPath == NULL) {
		std::cout << "You must provide the path to a valid Game Boy ROM file." << std::endl;
		printUsage(argv[0], false);
		return EXIT_FAILURE;
	}

	PlatformHeadless platform;

	GameBoy gb(romPath, platform);
	if(Config::get().useCustomBootRom) { gb.loadCustomBootRom(Config::get().customBootRomPath); }
	gb.debugCartridge();

	platform.addGameBoy(&gb);

	while(Config::get().skipIntro && !gb.introEnded()) {
		gb.tick();
	}

	while(running) {
		platform.beforeFrame();

		int cycles = 0;
		while(cycles < GameBoy::CYCLES_PER_FRAME) {
			cycles += gb.tick();
		}

		platform.showFrame();
		platform.afterFrame();
	}

	return EXIT_SUCCESS;
}
