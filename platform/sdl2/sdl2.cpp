#include <SDL.h>

#include "emulator.hpp"
#include "sdl2.hpp"

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

int main(int argc, char *argv[]) {
	PlatformSdl2 platform;
	Emulator emulator(platform);
	return emulator.run(argc, argv);
}
