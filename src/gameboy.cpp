#include "gameboy.hpp"
#include "cpu.hpp"
#include <cstdint>
#include <stdexcept>

void GameBoy::start(void) {
	if(m_cartridge->type() != 0) { throw std::runtime_error("\"ROM only\" cartridges are supported.\n"); }

	for(;;) {
		int cycles = m_cpu.executeInstruction();
	}
}

int GameBoy::tick() {
	m_cpu.handleInterrupts();
	int cycles = m_cpu.executeInstruction();
	assert(cycles % 4 == 0);

	for(size_t i = 0; i < cycles; i += 4) {
		m_ppu.tick(4);
		m_timer.tick(4);
		m_apu.tick(4);

		static uint8_t prevDiv = 0;
		uint8_t div = m_timer.getDiv();
		if((prevDiv & 0x10) == 0x10 && (div & 0x10) == 0) { m_apu.increaseDiv(); }
		prevDiv = div;
	}

	return cycles;
}
