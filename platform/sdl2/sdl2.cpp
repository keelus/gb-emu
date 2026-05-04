#include <SDL.h>
#include <cstdint>

#include "sdl2.hpp"

uint8_t activeColorPalette = 0;

PlatformSdl2::PlatformSdl2() {
	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		std::cerr << "Platform[SDL2]: SDL_Init error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	m_window = SDL_CreateWindow("Zirc Emulator", 100, 100, Lcd::WIDTH * SCALE, Lcd::HEIGHT * SCALE, SDL_WINDOW_SHOWN);
	if(!m_window) {
		std::cerr << "Platform[SDL2]: SDL_CreateWindow error: " << SDL_GetError() << std::endl;
		exit(1);
	}


	m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);
	if(!m_renderer) {
		std::cerr << "Platform[SDL2]: SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	m_texture =
		SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, Lcd::WIDTH, Lcd::HEIGHT);
	if(!m_texture) {
		std::cerr << "Platform[SDL2]: SDL_CreateTexture error: " << SDL_GetError() << std::endl;
		exit(1);
	}

	SDL_AudioSpec spec = {0};
	spec.freq = AUDIO_SAMPLE_RATE;
	spec.format = AUDIO_F32SYS;
	spec.channels = 1;
	spec.samples = AUDIO_SAMPLE_AMOUNT;
	spec.callback = audioCallback;
	spec.userdata = this;

	if(SDL_OpenAudio(&spec, NULL) < 0) {
		std::cerr << "Platform[SDL2]: SDL_OpenAudio error: " << SDL_GetError() << ". Audio is disabled." << std::endl;
	}
}

void printUsage(const char *argv0, const bool isHelpMessage) {
	std::cout << "Usage: " << argv0 << " [options] <ROM path>" << std::endl;
	std::cout << "Options:" << std::endl;
	std::cout << "\t -s, --serial\t\tEnable serial output" << std::endl;
	std::cout << "\t -f, --no-fps\t\tDisable 59.70fps limit" << std::endl;
	std::cout << "\t -d, --debug\t\tEnable debug output" << std::endl;
	std::cout << "\t -i, --skip-intro\tSkip boot intro" << std::endl;
	std::cout << "\t -h, --help\t\tShow " << (isHelpMessage ? "this" : "the") << " help message and exit" << std::endl;
}

int main(int argc, char *argv[]) {
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

	PlatformSdl2 platform;

	GameBoy gb(romPath, platform);
	gb.debugCartridge();

	platform.addGameBoy(&gb);

	while(Config::skipIntro && !gb.introEnded()) {
		gb.tick();
	}

	while(platform.running()) {
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
