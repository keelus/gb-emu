#include <iostream>
#include <SDL.h>
#include <SDL_ttf.h>
#include <unistd.h>
#include "ppu.hpp"

#include "gameboy.hpp"

#define SCALE 5

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

	int res = 0;

	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	SDL_Texture *texture = nullptr;

	SDL_Event e;
	int running = 1;

	if(SDL_Init(SDL_INIT_VIDEO) != 0) {
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

	while(running) {
		while(SDL_PollEvent(&e)) {
			if(e.type == SDL_QUIT) { running = 0; }
		}

		SDL_RenderClear(renderer);

		gb.tick(1000);

		SDL_UpdateTexture(texture, NULL, buffer, SCREEN_WIDTH * sizeof(uint32_t));
		SDL_RenderCopy(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
	}

quit:
	if(texture) SDL_DestroyTexture(texture);
	if(renderer) SDL_DestroyRenderer(renderer);
	if(window) SDL_DestroyWindow(window);
	TTF_Quit();
	SDL_Quit();
	return res;
}
