#include "gameboy.hpp"
#include "cpu.hpp"
#include <stdexcept>

void GameBoy::start(void) {
	m_catridge.debug();
	if(m_catridge.type() != 0) { throw std::runtime_error("ROM only catridges are supported.\n"); }

	for(;;) {
		int cycles = m_cpu.executeInstruction();
	}
}
