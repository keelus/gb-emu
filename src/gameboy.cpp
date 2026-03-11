#include "gameboy.hpp"
#include "cpu.hpp"
#include <stdexcept>

void GameBoy::start(void) {
	m_cartridge.debug();
	if(m_cartridge.type() != 0) { throw std::runtime_error("ROM only cartridges are supported.\n"); }

	for(;;) {
		int cycles = m_cpu.executeInstruction();
	}
}
