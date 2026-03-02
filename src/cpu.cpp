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
	case 0x03: // INC BC
		doInc16(m_B, m_C);
		break;
	case 0x04: // INC B
		doInc(m_B);
		break;
	case 0x05: // DEC B
		doDec(m_B);
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
	case 0x0C: // INC C
		doInc(m_C);
		break;
	case 0x0D: // DEC C
		doDec(m_C);
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
	case 0x13: // INC DE
		doInc16(m_D, m_E);
		break;
	case 0x14: // INC D
		doInc(m_D);
		break;
	case 0x15: // DEC D
		doDec(m_D);
		break;
	case 0x16: // LD D, imm8
		doLd(m_D, m_memory.read8(m_PC++));
		break;
	case 0x18: // JR imm8
		doJr();
		break;
	case 0x19: // LD HL, DE
		doAdd16ToHL(DE());
		break;
	case 0x1A: // LD A, [DE]
		doLd(m_A, m_memory.read8(DE()));
		break;
	case 0x1C: // INC E
		doInc(m_E);
		break;
	case 0x1D: // DEC E
		doDec(m_E);
		break;
	case 0x1E: // LD E, imm8
		doLd(m_E, m_memory.read8(m_PC++));
		break;

	case 0x20: // JR NZ, imm8
		doJr(!getFlag<Flag::Z>());
		break;
	case 0x21: // LD HL, imm16
		m_L = m_memory.read8(m_PC++);
		m_H = m_memory.read8(m_PC++);
		break;
	case 0x22: // LD [HL+], A
		m_memory.write8(HL(), m_A);
		incHL();
		break;
	case 0x23: // INC HL
		doInc16(m_H, m_L);
		break;
	case 0x24: // INC H
		doInc(m_H);
		break;
	case 0x25: // DEC H
		doDec(m_H);
		break;
	case 0x26: // LD H, imm8
		doLd(m_H, m_memory.read8(m_PC++));
		break;
	case 0x28: // JR Z, imm8
		doJr(getFlag<Flag::Z>());
		break;
	case 0x29: // LD HL, HL
		doAdd16ToHL(HL());
		break;
	case 0x2A: // LD A, [HL+]
		doLd(m_A, m_memory.read8(HL()));
		incHL();
		break;
	case 0x2C: // INC L
		doInc(m_L);
		break;
	case 0x2D: // DEC L
		doDec(m_L);
		break;
	case 0x2E: // LD L, imm8
		doLd(m_L, m_memory.read8(m_PC++));
		break;

	case 0x30: // JR NC, imm8
		doJr(!getFlag<Flag::C>());
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
	case 0x33: // INC SP
		doInc16(m_SP);
		break;
	case 0x34: { // INC [HL]
		doInc(HL());
		break;
	}
	case 0x35: // DEC [HL]
		doDec(HL());
		break;
	case 0x36: // LD [HL], imm8
		m_memory.write8(HL(), m_memory.read8(m_PC++));
		break;
	case 0x38: // JR C, imm8
		doJr(getFlag<Flag::C>());
		break;
	case 0x39: // LD HL, SP
		doAdd16ToHL(SP());
		break;
	case 0x3A: // LD A, [HL-]
		doLd(m_A, m_memory.read8(HL()));
		decHL();
		break;
	case 0x3C: // INC A
		doInc(m_A);
		break;
	case 0x3D: // DEC A
		doDec(m_A);
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
	case 0x4A: // LD C, D
		doLd(m_C, m_D);
		break;
	case 0x4B: // LD C, E
		doLd(m_C, m_E);
		break;
	case 0x4C: // LD C, H
		doLd(m_C, m_H);
		break;
	case 0x4D: // LD C, L
		doLd(m_C, m_L);
		break;
	case 0x4E: // LD C, [HL]
		doLd(m_C, m_memory.read8(HL()));
		break;
	case 0x4F: // LD C, A
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
	case 0x5A: // LD E, D
		doLd(m_E, m_D);
		break;
	case 0x5B: // LD E, E
		doLd(m_E, m_E);
		break;
	case 0x5C: // LD E, H
		doLd(m_E, m_H);
		break;
	case 0x5D: // LD E, L
		doLd(m_E, m_L);
		break;
	case 0x5E: // LD E, [HL]
		doLd(m_E, m_memory.read8(HL()));
		break;
	case 0x5F: // LD E, A
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
	case 0x6A: // LD L, D
		doLd(m_L, m_D);
		break;
	case 0x6B: // LD L, E
		doLd(m_L, m_E);
		break;
	case 0x6C: // LD L, H
		doLd(m_L, m_H);
		break;
	case 0x6D: // LD L, L
		doLd(m_L, m_L);
		break;
	case 0x6E: // LD L, [HL]
		doLd(m_L, m_memory.read8(HL()));
		break;
	case 0x6F: // LD L, A
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
	case 0x7A: // LD A, D
		doLd(m_A, m_D);
		break;
	case 0x7B: // LD A, E
		doLd(m_A, m_E);
		break;
	case 0x7C: // LD A, H
		doLd(m_A, m_H);
		break;
	case 0x7D: // LD A, L
		doLd(m_A, m_L);
		break;
	case 0x7E: // LD A, [HL]
		doLd(m_A, m_memory.read8(HL()));
		break;
	case 0x7F: // LD A, A
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
	case 0x90: // SUB A, B
		doSub(m_A, m_B);
		break;
	case 0x91: // SUB A, C
		doSub(m_A, m_C);
		break;
	case 0x92: // SUB A, D
		doSub(m_A, m_D);
		break;
	case 0x93: // SUB A, E
		doSub(m_A, m_E);
		break;
	case 0x94: // SUB A, H
		doSub(m_A, m_H);
		break;
	case 0x95: // SUB A, L
		doSub(m_A, m_L);
		break;
	case 0x96: // SUB A, [HL]
		doSub(m_A, m_memory.read8(HL()));
		break;
	case 0x97: // SUB A, A
		doSub(m_A, m_A);
		break;
	case 0x98: // SBC A, B
		doSbc(m_A, m_B);
		break;
	case 0x99: // SBC A, C
		doSbc(m_A, m_C);
		break;
	case 0x9A: // SBC A, D
		doSbc(m_A, m_D);
		break;
	case 0x9B: // SBC A, E
		doSbc(m_A, m_E);
		break;
	case 0x9C: // SBC A, H
		doSbc(m_A, m_H);
		break;
	case 0x9D: // SBC A, L
		doSbc(m_A, m_L);
		break;
	case 0x9E: // SBC A, [HL]
		doSbc(m_A, m_memory.read8(HL()));
		break;
	case 0x9F: // SBC A, A
		doSbc(m_A, m_A);
		break;
	case 0xA0: // AND A, B
		doAnd(m_A, m_B);
		break;
	case 0xA1: // AND A, C
		doAnd(m_A, m_C);
		break;
	case 0xA2: // AND A, D
		doAnd(m_A, m_D);
		break;
	case 0xA3: // AND A, E
		doAnd(m_A, m_E);
		break;
	case 0xA4: // AND A, H
		doAnd(m_A, m_H);
		break;
	case 0xA5: // AND A, L
		doAnd(m_A, m_L);
		break;
	case 0xA6: // AND A, [HL]
		doAnd(m_A, m_memory.read8(HL()));
		break;
	case 0xA7: // AND A, A
		doAnd(m_A, m_A);
		break;
	case 0xA8: // XOR A, B
		doXor(m_A, m_B);
		break;
	case 0xA9: // XOR A, C
		doXor(m_A, m_C);
		break;
	case 0xAA: // XOR A, D
		doXor(m_A, m_D);
		break;
	case 0xAB: // XOR A, E
		doXor(m_A, m_E);
		break;
	case 0xAC: // XOR A, H
		doXor(m_A, m_H);
		break;
	case 0xAD: // XOR A, L
		doXor(m_A, m_L);
		break;
	case 0xAE: // XOR A, [HL]
		doXor(m_A, m_memory.read8(HL()));
		break;
	case 0xAF: // XOR A, A
		doXor(m_A, m_A);
		break;
	case 0xB0: // OR A, B
		doOr(m_A, m_B);
		break;
	case 0xB1: // OR A, C
		doOr(m_A, m_C);
		break;
	case 0xB2: // OR A, D
		doOr(m_A, m_D);
		break;
	case 0xB3: // OR A, E
		doOr(m_A, m_E);
		break;
	case 0xB4: // OR A, H
		doOr(m_A, m_H);
		break;
	case 0xB5: // OR A, L
		doOr(m_A, m_L);
		break;
	case 0xB6: // OR A, [HL]
		doOr(m_A, m_memory.read8(HL()));
		break;
	case 0xB7: // OR A, A
		doOr(m_A, m_A);
		break;
	case 0xB8: // CP A, B
		doCp(m_A, m_B);
		break;
	case 0xB9: // CP A, C
		doCp(m_A, m_C);
		break;
	case 0xBA: // CP A, D
		doCp(m_A, m_D);
		break;
	case 0xBB: // CP A, E
		doCp(m_A, m_E);
		break;
	case 0xBC: // CP A, H
		doCp(m_A, m_H);
		break;
	case 0xBD: // CP A, L
		doCp(m_A, m_L);
		break;
	case 0xBE: // CP A, [HL]
		doCp(m_A, m_memory.read8(HL()));
		break;
	case 0xBF: // CP A, A
		doCp(m_A, m_A);
		break;
	case 0xC0: // RET NZ
		doRet(!getFlag<Flag::Z>());
		break;
	case 0xC1: // POP BC
		doPop(m_B, m_C);
		break;
	case 0xC2: // JP NZ, imm16
		doJp(!getFlag<Flag::Z>());
		break;
	case 0xC3: // JP imm16
		doJp();
		break;
	case 0xC4: // CALL NZ, imm16
		doCall(!getFlag<Flag::Z>());
		break;
	case 0xC5: // PUSH BC
		doPush(BC());
		break;
	case 0xC6: // ADD A, imm8
		doAdd(m_A, m_memory.read8(m_PC++));
		break;
	case 0xC8: // RET Z
		doRet(getFlag<Flag::Z>());
		break;
	case 0xC9: // RET
		doRet();
		break;
	case 0xCA: // JP Z, imm16
		doJp(getFlag<Flag::Z>());
		break;
	case 0xCC: // CALL Z, imm16
		doCall(getFlag<Flag::Z>());
		break;
	case 0xCD: // CALL imm16
		doCall();
		break;
	case 0xCE: // ADC A, imm8
		doAdc(m_A, m_memory.read8(m_PC++));
		break;

	case 0xD0: // RET NC
		doRet(!getFlag<Flag::C>());
		break;
	case 0xD1: // POP DE
		doPop(m_D, m_E);
		break;
	case 0xD2: // JP NC, imm16
		doJp(!getFlag<Flag::C>());
		break;
	case 0xD4: // CALL NC, imm16
		doCall(!getFlag<Flag::C>());
		break;
	case 0xD5: // PUSH DE
		doPush(DE());
		break;
	case 0xD6: // SUB A, imm8
		doSub(m_A, m_memory.read8(m_PC++));
		break;
	case 0xD8: // RET C
		doRet(getFlag<Flag::C>());
		break;
	case 0xDA: // JP C, imm16
		doJp(getFlag<Flag::C>());
		break;
	case 0xDC: // CALL C, imm16
		doCall(getFlag<Flag::C>());
		break;
	case 0xDE: // SBC A, imm8
		doSbc(m_A, m_memory.read8(m_PC++));
		break;

	case 0xE0: { // LDH [imm8], A
		uint8_t low = m_memory.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		m_memory.write8(address, m_A);
		break;
	}
	case 0xE1: // POP HL
		doPop(m_H, m_L);
		break;
	case 0xE2: { // LDH [C], A
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		m_memory.write8(address, m_A);
		break;
	}
	case 0xE5: // PUSH HL
		doPush(HL());
		break;
	case 0xE6: // AND A, imm8
		doAnd(m_A, m_memory.read8(m_PC++));
		break;
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
	case 0xE9: // JP HL
		m_PC = HL();
		break;
	case 0xEA: { // LD [imm16], A
		uint8_t low = m_memory.read8(m_PC++);
		uint8_t high = m_memory.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		m_memory.write8(address, m_A);
		break;
	}
	case 0xEE: // XOR A, imm8
		doXor(m_A, m_memory.read8(m_PC++));
		break;

	case 0xF0: { // LDH A, [imm8]
		uint8_t low = m_memory.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		doLd(m_A, m_memory.read8(address));
		break;
	}
	case 0xF1: // POP AF
		doPop(m_A, m_F);
		break;
	case 0xF2: { // LDH A, [C]
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		doLd(m_A, m_memory.read8(address));
		break;
	}
	case 0xF5: // PUSH AF
		doPush(AF());
		break;
	case 0xF6: // OR A, imm8
		doOr(m_A, m_memory.read8(m_PC++));
		break;
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
	case 0xFE: // CP A, imm8
		doCp(m_A, m_memory.read8(m_PC++));
		break;

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
