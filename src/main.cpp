#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include "SDL_events.h"
#include "SDL_keycode.h"
#include "SDL_timer.h"
#include "graphics/lcd.hpp"
#include "graphics/ppu.hpp"
#include "config.hpp"
#include "gameboy.hpp"

#define SCALE 5

#define CPU_HZ 4194304
#define FRAMES_PER_SECOND 59.7f
#define CYCLES_PER_FRAME (CPU_HZ / FRAMES_PER_SECOND)
#define MS_PER_FRAME (1 / FRAMES_PER_SECOND * 1000)

uint8_t activeColorPalette = 2;

void printUsage(const char *argv0, const bool isHelpMessage) {
	std::cout << "Usage:" << argv0 << " [options] <ROM path>" << std::endl;
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

	GameBoy gb(romPath);
	gb.debugCartridge();
	std::cout << "Press any key to start the Game Boy..." << std::endl;
	std::cin.get();

	int res = 0;

	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	SDL_Texture *texture = nullptr;

	SDL_Event e;
	int running = 1;

	if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		std::cerr << "SDL: SDL_Init error: " << SDL_GetError() << std::endl;
		res = 1;
		goto quit;
	}

	if(TTF_Init() != 0) {
		std::cerr << "SDL_ttf: TTF_Init error: " << TTF_GetError() << std::endl;
		res = 1;
		goto quit;
	}

	window = SDL_CreateWindow("Black Window", 100, 100, SCREEN_WIDTH * SCALE, SCREEN_HEIGHT * SCALE, SDL_WINDOW_SHOWN);
	if(!window) {
		std::cerr << "SDL: SDL_CreateWindow error: " << SDL_GetError() << std::endl;
		res = 1;
		goto quit;
	}


	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer) {
		std::cerr << "SDL: SDL_CreateRenderer error: " << SDL_GetError() << std::endl;
		goto quit;
	}

	texture =
		SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
	if(!texture) {
		std::cerr << "SDL: SDL_CreateTexture error: " << SDL_GetError() << std::endl;
		goto quit;
	}

	while(Config::skipIntro && !gb.introEnded()) {
		gb.tick();
	}

	while(running) {
		Uint32 frameStart = SDL_GetTicks();
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) {
				running = 0;
			} else if(e.type == SDL_KEYDOWN) {
				switch(e.key.keysym.sym) {
				case SDLK_1: activeColorPalette = 0; break;
				case SDLK_2: activeColorPalette = 1; break;
				case SDLK_3: activeColorPalette = 2; break;
				default: break;
				}

				gb.handleKeydown(e.key.keysym.sym);
			} else if(e.type == SDL_KEYUP) {
				gb.handleKeyup(e.key.keysym.sym);
			}
		}

		SDL_RenderClear(renderer);

		int cycles = 0;
		while(cycles < CYCLES_PER_FRAME) {
			cycles += gb.tick();
		}

		SDL_UpdateTexture(texture, NULL, gb.getLcdBuffer(), SCREEN_WIDTH * sizeof(uint32_t));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);

		if(Config::limitFps) {
			Uint32 frameMs = SDL_GetTicks() - frameStart;
			if(frameMs < MS_PER_FRAME) { SDL_Delay(MS_PER_FRAME - frameMs); }
		}
	}

quit:
	gb.dump();
	if(texture) SDL_DestroyTexture(texture);
	if(renderer) SDL_DestroyRenderer(renderer);
	if(window) SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return res;
}
