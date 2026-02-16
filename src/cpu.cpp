#include "cpu.hpp"
#include "cpu_instructions.hpp"
#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>

int Cpu::executeInstruction(void) {
	if(m_halted) { return 0; }

	uint8_t opcode = m_memory.read8(m_PC);
	std::cout << "Got instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << " at PC=0x"
			  << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
			  << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"" << std::endl;

	switch(opcode) {
	case 0x00: // NOP
		break;
	case 0x76: // HALT
		m_halted = true;
		break;
	default: {
		std::stringstream ss;
		ss << "Unhandled instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << " at PC=0x"
		   << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
		   << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"";

		throw std::runtime_error(ss.str());
	}
	}

	m_PC++;

	return CPU_INSTRUCTION_CYCLES.at(opcode);
}
