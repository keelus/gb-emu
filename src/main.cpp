#include "gameboy.hpp"

int main(void) {
	GameBoy gb("../gb-roms/tetris.gb");
	gb.start();

	return 0;
}
