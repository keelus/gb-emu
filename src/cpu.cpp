#include "cpu.hpp"
#include "cpu_instructions.hpp"
#include <cstdint>
#include <iomanip>
#include <ios>
#include <sstream>

int Cpu::executeInstruction(void) {
	if(m_halted) { return 0; }

	uint8_t opcode = m_memory.read8(m_PC++);
	std::cout << "Got instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << " at PC=0x"
			  << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
			  << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"" << std::endl;

	switch(opcode) {
	case 0x00: // NOP
		break;
	case 0x01: // LD BC, imm16
		m_C = m_memory.read8(m_PC++);
		m_B = m_memory.read8(m_PC++);
		break;
	case 0x02: // LD [BC], A
		m_memory.write8(BC(), m_A);
		break;
	case 0x06: // LD B, imm8
		doLd(m_B, m_memory.read8(m_PC++));
		break;
	case 0x08: { // LD [imm16], SP
		uint8_t low = m_memory.read8(m_PC++);
		uint8_t high = m_memory.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		m_memory.write16(address, m_SP);
		break;
	}
	case 0x09: // LD HL, BC
		doAdd16ToHL(BC());
		break;
	case 0x0A: // LD A, [BC]
		doLd(m_A, m_memory.read8(BC()));
		break;
	case 0x0E: // LD C, imm8
		doLd(m_C, m_memory.read8(m_PC++));
		break;

	case 0x11: // LD DE, imm16
		m_E = m_memory.read8(m_PC++);
		m_D = m_memory.read8(m_PC++);
		break;
	case 0x12: // LD [DE], A
		m_memory.write8(DE(), m_A);
		break;
	case 0x16: // LD D, imm8
		doLd(m_D, m_memory.read8(m_PC++));
		break;
	case 0x19: // LD HL, DE
		doAdd16ToHL(DE());
		break;
	case 0x1A: // LD A, [DE]
		doLd(m_A, m_memory.read8(DE()));
		break;
	case 0x1E: // LD E, imm8
		doLd(m_E, m_memory.read8(m_PC++));
		break;

	case 0x21: // LD HL, imm16
		m_L = m_memory.read8(m_PC++);
		m_H = m_memory.read8(m_PC++);
		break;
	case 0x22: // LD [HL+], A
		m_memory.write8(HL(), m_A);
		incHL();
		break;
	case 0x26: // LD H, imm8
		doLd(m_H, m_memory.read8(m_PC++));
		break;
	case 0x29: // LD HL, HL
		doAdd16ToHL(HL());
		break;
	case 0x2A: // LD A, [HL+]
		doLd(m_A, m_memory.read8(HL()));
		incHL();
		break;
	case 0x2E: // LD L, imm8
		doLd(m_L, m_memory.read8(m_PC++));
		break;

	case 0x31: { // LD SP, imm16
		uint8_t low = m_memory.read8(m_PC++);
		uint8_t high = m_memory.read8(m_PC++);
		m_SP = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		break;
	}
	case 0x32: // LD [HL-], A
		m_memory.write8(HL(), m_A);
		decHL();
		break;
	case 0x36: // LD [HL], imm8
		m_memory.write8(HL(), m_memory.read8(m_PC++));
		break;
	case 0x39: // LD HL, SP
		doAdd16ToHL(SP());
		break;
	case 0x3A: // LD A, [HL-]
		doLd(m_A, m_memory.read8(HL()));
		decHL();
		break;
	case 0x3E: // LD A, imm8
		doLd(m_A, m_memory.read8(m_PC++));
		break;

	case 0x40: // LD B, B
		doLd(m_B, m_B);
		break;
	case 0x41: // LD B, C
		doLd(m_B, m_C);
		break;
	case 0x42: // LD B, D
		doLd(m_B, m_D);
		break;
	case 0x43: // LD B, E
		doLd(m_B, m_E);
		break;
	case 0x44: // LD B, H
		doLd(m_B, m_H);
		break;
	case 0x45: // LD B, L
		doLd(m_B, m_L);
		break;
	case 0x46: // LD B, [HL]
		doLd(m_B, m_memory.read8(HL()));
		break;
	case 0x47: // LD B, A
		doLd(m_B, m_A);
		break;
	case 0x48: // LD C, B
		doLd(m_C, m_B);
		break;
	case 0x49: // LD C, C
		doLd(m_C, m_C);
		break;
	case 0x4a: // LD C, D
		doLd(m_C, m_D);
		break;
	case 0x4b: // LD C, E
		doLd(m_C, m_E);
		break;
	case 0x4c: // LD C, H
		doLd(m_C, m_H);
		break;
	case 0x4d: // LD C, L
		doLd(m_C, m_L);
		break;
	case 0x4e: // LD C, [HL]
		doLd(m_C, m_memory.read8(HL()));
		break;
	case 0x4f: // LD C, A
		doLd(m_C, m_A);
		break;
	case 0x50: // LD D, B
		doLd(m_D, m_B);
		break;
	case 0x51: // LD D, C
		doLd(m_D, m_C);
		break;
	case 0x52: // LD D, D
		doLd(m_D, m_D);
		break;
	case 0x53: // LD D, E
		doLd(m_D, m_E);
		break;
	case 0x54: // LD D, H
		doLd(m_D, m_H);
		break;
	case 0x55: // LD D, L
		doLd(m_D, m_L);
		break;
	case 0x56: // LD D, [HL]
		doLd(m_D, m_memory.read8(HL()));
		break;
	case 0x57: // LD D, A
		doLd(m_D, m_A);
		break;
	case 0x58: // LD E, B
		doLd(m_E, m_B);
		break;
	case 0x59: // LD E, C
		doLd(m_E, m_C);
		break;
	case 0x5a: // LD E, D
		doLd(m_E, m_D);
		break;
	case 0x5b: // LD E, E
		doLd(m_E, m_E);
		break;
	case 0x5c: // LD E, H
		doLd(m_E, m_H);
		break;
	case 0x5d: // LD E, L
		doLd(m_E, m_L);
		break;
	case 0x5e: // LD E, [HL]
		doLd(m_E, m_memory.read8(HL()));
		break;
	case 0x5f: // LD E, A
		doLd(m_E, m_A);
		break;
	case 0x60: // LD H, B
		doLd(m_H, m_B);
		break;
	case 0x61: // LD H, C
		doLd(m_H, m_C);
		break;
	case 0x62: // LD H, D
		doLd(m_H, m_D);
		break;
	case 0x63: // LD H, E
		doLd(m_H, m_E);
		break;
	case 0x64: // LD H, H
		doLd(m_H, m_H);
		break;
	case 0x65: // LD H, L
		doLd(m_H, m_L);
		break;
	case 0x66: // LD H, [HL]
		doLd(m_H, m_memory.read8(HL()));
		break;
	case 0x67: // LD H, A
		doLd(m_H, m_A);
		break;
	case 0x68: // LD L, B
		doLd(m_L, m_B);
		break;
	case 0x69: // LD L, C
		doLd(m_L, m_C);
		break;
	case 0x6a: // LD L, D
		doLd(m_L, m_D);
		break;
	case 0x6b: // LD L, E
		doLd(m_L, m_E);
		break;
	case 0x6c: // LD L, H
		doLd(m_L, m_H);
		break;
	case 0x6d: // LD L, L
		doLd(m_L, m_L);
		break;
	case 0x6e: // LD L, [HL]
		doLd(m_L, m_memory.read8(HL()));
		break;
	case 0x6f: // LD L, A
		doLd(m_L, m_A);
		break;
	case 0x70: // LD [HL], B
		m_memory.write8(HL(), m_B);
		break;
	case 0x71: // LD [HL], C
		m_memory.write8(HL(), m_C);
		break;
	case 0x72: // LD [HL], D
		m_memory.write8(HL(), m_D);
		break;
	case 0x73: // LD [HL], E
		m_memory.write8(HL(), m_E);
		break;
	case 0x74: // LD [HL], H
		m_memory.write8(HL(), m_H);
		break;
	case 0x75: // LD [HL], L
		m_memory.write8(HL(), m_L);
		break;
	case 0x76: // HALT
		m_halted = true;
		break;
	case 0x77: // LD [HL], A
		m_memory.write8(HL(), m_A);
		break;
	case 0x78: // LD A, B
		doLd(m_A, m_B);
		break;
	case 0x79: // LD A, C
		doLd(m_A, m_C);
		break;
	case 0x7a: // LD A, D
		doLd(m_A, m_D);
		break;
	case 0x7b: // LD A, E
		doLd(m_A, m_E);
		break;
	case 0x7c: // LD A, H
		doLd(m_A, m_H);
		break;
	case 0x7d: // LD A, L
		doLd(m_A, m_L);
		break;
	case 0x7e: // LD A, [HL]
		doLd(m_A, m_memory.read8(HL()));
		break;
	case 0x7f: // LD A, A
		doLd(m_A, m_A);
		break;

	case 0x80: // ADD A, B
		doAdd(m_A, m_B);
		break;
	case 0x81: // ADD A, C
		doAdd(m_A, m_C);
		break;
	case 0x82: // ADD A, D
		doAdd(m_A, m_D);
		break;
	case 0x83: // ADD A, E
		doAdd(m_A, m_E);
		break;
	case 0x84: // ADD A, H
		doAdd(m_A, m_H);
		break;
	case 0x85: // ADD A, L
		doAdd(m_A, m_L);
		break;
	case 0x86: // ADD A, [HL]
		doAdd(m_A, m_memory.read8(HL()));
		break;
	case 0x87: // ADD A, A
		doAdd(m_A, m_A);
		break;
	case 0x88: // ADC A, B
		doAdc(m_A, m_B);
		break;
	case 0x89: // ADC A, C
		doAdc(m_A, m_C);
		break;
	case 0x8A: // ADC A, D
		doAdc(m_A, m_D);
		break;
	case 0x8B: // ADC A, E
		doAdc(m_A, m_E);
		break;
	case 0x8C: // ADC A, H
		doAdc(m_A, m_H);
		break;
	case 0x8D: // ADC A, L
		doAdc(m_A, m_L);
		break;
	case 0x8E: // ADC A, [HL]
		doAdc(m_A, m_memory.read8(HL()));
		break;
	case 0x8F: // ADC A, A
		doAdc(m_A, m_A);
		break;

	case 0xC6: // ADD A, imm8
		doAdd(m_A, m_memory.read8(m_PC++));
		break;
	case 0xCE: // ADC A, imm8
		doAdc(m_A, m_memory.read8(m_PC++));
		break;

	case 0xE0: { // LDH [imm8], A
		uint8_t low = m_memory.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		m_memory.write8(address, m_A);
		break;
	}
	case 0xE2: { // LDH [C], A
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		m_memory.write8(address, m_A);
		break;
	}
	case 0xE8: { // ADD SP, e8
		uint16_t imm8 = static_cast<uint16_t>(m_memory.read8(m_PC++));
		uint16_t result = (m_SP + imm8);

		setFlag<Cpu::Flag::Z>(0);
		setFlag<Cpu::Flag::N>(0);
		setFlag<Cpu::Flag::H>(((m_SP ^ imm8 ^ result) & (1 << 4)) == (1 << 4));
		setFlag<Cpu::Flag::C>(((m_SP ^ imm8 ^ result) & (1 << 8)) == (1 << 8));

		m_SP = result;
		break;
	}
	case 0xEA: { // LD [imm16], A
		uint8_t low = m_memory.read8(m_PC++);
		uint8_t high = m_memory.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		m_memory.write8(address, m_A);
		break;
	}

	case 0xF0: { // LDH A, [imm8]
		uint8_t low = m_memory.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		doLd(m_A, m_memory.read8(address));
		break;
	}
	case 0xF2: { // LDH A, [C]
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		doLd(m_A, m_memory.read8(address));
		break;
	}
	case 0xF8: { // LD HL, SP+e8
		uint16_t imm8 = static_cast<uint16_t>(m_memory.read8(m_PC++));
		uint16_t result = (m_SP + imm8);

		m_H = static_cast<uint8_t>(static_cast<uint16_t>(result) >> 8);
		m_L = static_cast<uint8_t>(static_cast<uint16_t>(result));

		setFlag<Cpu::Flag::Z>(0);
		setFlag<Cpu::Flag::N>(0);
		setFlag<Cpu::Flag::H>(((m_SP ^ imm8 ^ result) & (1 << 4)) == (1 << 4));
		setFlag<Cpu::Flag::C>(((m_SP ^ imm8 ^ result) & (1 << 8)) == (1 << 8));

		break;
	}
	case 0xF9: { // LD SP, HL
		m_SP = HL();
		break;
	}
	case 0xFA: { // LD A, [imm16]
		uint8_t low = m_memory.read8(m_PC++);
		uint8_t high = m_memory.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		doLd(m_A, m_memory.read8(address));
		break;
	}

	default: {
		std::stringstream ss;
		ss << "Unhandled instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << " at PC=0x"
		   << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
		   << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"";

		throw std::runtime_error(ss.str());
	}
	}

	return CPU_INSTRUCTION_CYCLES.at(opcode);
}
