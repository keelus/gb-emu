#include "gameboy.hpp"
#include "cpu.hpp"
#include <stdexcept>

void GameBoy::start(void) {
	if(m_cartridge.type() != 0) { throw std::runtime_error("\"ROM only\" cartridges are supported.\n"); }

	for(;;) {
		int cycles = m_cpu.executeInstruction();
	}
}

void GameBoy::tick(size_t tickCycles) {
	for(size_t i = 0; i < tickCycles; i++) {
		int cycles = m_cpu.executeInstruction();
		m_ppu.tick(cycles);
	}
}
