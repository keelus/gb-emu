#include <cstdint>
#include <stdexcept>

#include "cpu/cpu.hpp"
#include "gameboy.hpp"

namespace Zirc {
void GameBoy::start(void) {
	if(m_cartridge->type() != 0) { throw std::runtime_error("\"ROM only\" cartridges are supported.\n"); }

	for(;;) {
		m_cpu.executeInstruction();
	}
}

int GameBoy::tick() {
	m_cpu.handleInterrupts();
	uint8_t cycles = m_cpu.executeInstruction();
	assert(cycles % 4 == 0);

	for(uint8_t i = 0; i < cycles; i += 4) {
		m_ppu.tick(4);
		m_timer.tick(4);
		m_apu.tick(4);

		uint8_t div = m_timer.getDiv();
		if((m_prevDiv & 0x10) == 0x10 && (div & 0x10) == 0) { m_apu.increaseDiv(); }
		m_prevDiv = div;
	}

	return cycles;
}
} // namespace Zirc
