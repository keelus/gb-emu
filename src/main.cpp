#include "cpu.hpp"
#include "catridge.hpp"

int main(void) {
	Cpu cpu;
	Catridge catridge("../gb-roms/tetris.gb");
	catridge.debug();
	return 0;
}
