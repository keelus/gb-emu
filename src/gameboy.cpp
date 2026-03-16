#include "gameboy.hpp"
#include "cpu.hpp"
#include <stdexcept>

void GameBoy::start(void) {
	if(m_cartridge.type() != 0) { throw std::runtime_error("\"ROM only\" cartridges are supported.\n"); }

	for(;;) {
		int cycles = m_cpu.executeInstruction();
	}
}

int GameBoy::tick() {
	int cycles = m_cpu.executeInstruction();
	m_ppu.tick(cycles);

	return cycles;
}
