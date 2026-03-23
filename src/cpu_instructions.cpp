#include <array>
#include "cpu.hpp"
#include <cassert>
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>

// Useful to visually see the OPCODE-MNEMONIC/CYCLE
#define OP(op, data) data

// clang-format off
constexpr std::array<const char *, 256> CPU_INSTRUCTION_MNEMONICS = {
	OP(0x00, "NOP"),
	OP(0x01, "LD BC, n16"),
	OP(0x02, "LD [BC], A"),
	OP(0x03, "INC BC"),
	OP(0x04, "INC B"),
	OP(0x05, "DEC B"),
	OP(0x06, "LD B, n8"),
	OP(0x07, "RLCA"),
	OP(0x08, "LD [a16], SP"),
	OP(0x09, "ADD HL, BC"),
	OP(0x0A, "LD A, [BC]"),
	OP(0x0B, "DEC BC"),
	OP(0x0C, "INC C"),
	OP(0x0D, "DEC C"),
	OP(0x0E, "LD C, n8"),
	OP(0x0F, "RRCA"),
	OP(0x10, "STOP n8"),
	OP(0x11, "LD DE, n16"),
	OP(0x12, "LD [DE], A"),
	OP(0x13, "INC DE"),
	OP(0x14, "INC D"),
	OP(0x15, "DEC D"),
	OP(0x16, "LD D, n8"),
	OP(0x17, "RLA"),
	OP(0x18, "JR e8"),
	OP(0x19, "ADD HL, DE"),
	OP(0x1A, "LD A, [DE]"),
	OP(0x1B, "DEC DE"),
	OP(0x1C, "INC E"),
	OP(0x1D, "DEC E"),
	OP(0x1E, "LD E, n8"),
	OP(0x1F, "RRA"),
	OP(0x20, "JR NZ, e8"),
	OP(0x21, "LD HL, n16"),
	OP(0x22, "LD [HL+], A"),
	OP(0x23, "INC HL"),
	OP(0x24, "INC H"),
	OP(0x25, "DEC H"),
	OP(0x26, "LD H, n8"),
	OP(0x27, "DAA"),
	OP(0x28, "JR Z, e8"),
	OP(0x29, "ADD HL, HL"),
	OP(0x2A, "LD A, [HL+]"),
	OP(0x2B, "DEC HL"),
	OP(0x2C, "INC L"),
	OP(0x2D, "DEC L"),
	OP(0x2E, "LD L, n8"),
	OP(0x2F, "CPL"),
	OP(0x30, "JR NC, e8"),
	OP(0x31, "LD SP, n16"),
	OP(0x32, "LD [HL-], A"),
	OP(0x33, "INC SP"),
	OP(0x34, "INC [HL]"),
	OP(0x35, "DEC [HL]"),
	OP(0x36, "LD [HL], n8"),
	OP(0x37, "SCF"),
	OP(0x38, "JR C, e8"),
	OP(0x39, "ADD HL, SP"),
	OP(0x3A, "LD A, [HL-]"),
	OP(0x3B, "DEC SP"),
	OP(0x3C, "INC A"),
	OP(0x3D, "DEC A"),
	OP(0x3E, "LD A, n8"),
	OP(0x3F, "CCF"),
	OP(0x40, "LD B, B"),
	OP(0x41, "LD B, C"),
	OP(0x42, "LD B, D"),
	OP(0x43, "LD B, E"),
	OP(0x44, "LD B, H"),
	OP(0x45, "LD B, L"),
	OP(0x46, "LD B, [HL]"),
	OP(0x47, "LD B, A"),
	OP(0x48, "LD C, B"),
	OP(0x49, "LD C, C"),
	OP(0x4A, "LD C, D"),
	OP(0x4B, "LD C, E"),
	OP(0x4C, "LD C, H"),
	OP(0x4D, "LD C, L"),
	OP(0x4E, "LD C, [HL]"),
	OP(0x4F, "LD C, A"),
	OP(0x50, "LD D, B"),
	OP(0x51, "LD D, C"),
	OP(0x52, "LD D, D"),
	OP(0x53, "LD D, E"),
	OP(0x54, "LD D, H"),
	OP(0x55, "LD D, L"),
	OP(0x56, "LD D, [HL]"),
	OP(0x57, "LD D, A"),
	OP(0x58, "LD E, B"),
	OP(0x59, "LD E, C"),
	OP(0x5A, "LD E, D"),
	OP(0x5B, "LD E, E"),
	OP(0x5C, "LD E, H"),
	OP(0x5D, "LD E, L"),
	OP(0x5E, "LD E, [HL]"),
	OP(0x5F, "LD E, A"),
	OP(0x60, "LD H, B"),
	OP(0x61, "LD H, C"),
	OP(0x62, "LD H, D"),
	OP(0x63, "LD H, E"),
	OP(0x64, "LD H, H"),
	OP(0x65, "LD H, L"),
	OP(0x66, "LD H, [HL]"),
	OP(0x67, "LD H, A"),
	OP(0x68, "LD L, B"),
	OP(0x69, "LD L, C"),
	OP(0x6A, "LD L, D"),
	OP(0x6B, "LD L, E"),
	OP(0x6C, "LD L, H"),
	OP(0x6D, "LD L, L"),
	OP(0x6E, "LD L, [HL]"),
	OP(0x6F, "LD L, A"),
	OP(0x70, "LD [HL], B"),
	OP(0x71, "LD [HL], C"),
	OP(0x72, "LD [HL], D"),
	OP(0x73, "LD [HL], E"),
	OP(0x74, "LD [HL], H"),
	OP(0x75, "LD [HL], L"),
	OP(0x76, "HALT"),
	OP(0x77, "LD [HL], A"),
	OP(0x78, "LD A, B"),
	OP(0x79, "LD A, C"),
	OP(0x7A, "LD A, D"),
	OP(0x7B, "LD A, E"),
	OP(0x7C, "LD A, H"),
	OP(0x7D, "LD A, L"),
	OP(0x7E, "LD A, [HL]"),
	OP(0x7F, "LD A, A"),
	OP(0x80, "ADD A, B"),
	OP(0x81, "ADD A, C"),
	OP(0x82, "ADD A, D"),
	OP(0x83, "ADD A, E"),
	OP(0x84, "ADD A, H"),
	OP(0x85, "ADD A, L"),
	OP(0x86, "ADD A, [HL]"),
	OP(0x87, "ADD A, A"),
	OP(0x88, "ADC A, B"),
	OP(0x89, "ADC A, C"),
	OP(0x8A, "ADC A, D"),
	OP(0x8B, "ADC A, E"),
	OP(0x8C, "ADC A, H"),
	OP(0x8D, "ADC A, L"),
	OP(0x8E, "ADC A, [HL]"),
	OP(0x8F, "ADC A, A"),
	OP(0x90, "SUB A, B"),
	OP(0x91, "SUB A, C"),
	OP(0x92, "SUB A, D"),
	OP(0x93, "SUB A, E"),
	OP(0x94, "SUB A, H"),
	OP(0x95, "SUB A, L"),
	OP(0x96, "SUB A, [HL]"),
	OP(0x97, "SUB A, A"),
	OP(0x98, "SBC A, B"),
	OP(0x99, "SBC A, C"),
	OP(0x9A, "SBC A, D"),
	OP(0x9B, "SBC A, E"),
	OP(0x9C, "SBC A, H"),
	OP(0x9D, "SBC A, L"),
	OP(0x9E, "SBC A, [HL]"),
	OP(0x9F, "SBC A, A"),
	OP(0xA0, "AND A, B"),
	OP(0xA1, "AND A, C"),
	OP(0xA2, "AND A, D"),
	OP(0xA3, "AND A, E"),
	OP(0xA4, "AND A, H"),
	OP(0xA5, "AND A, L"),
	OP(0xA6, "AND A, [HL]"),
	OP(0xA7, "AND A, A"),
	OP(0xA8, "XOR A, B"),
	OP(0xA9, "XOR A, C"),
	OP(0xAA, "XOR A, D"),
	OP(0xAB, "XOR A, E"),
	OP(0xAC, "XOR A, H"),
	OP(0xAD, "XOR A, L"),
	OP(0xAE, "XOR A, [HL]"),
	OP(0xAF, "XOR A, A"),
	OP(0xB0, "OR A, B"),
	OP(0xB1, "OR A, C"),
	OP(0xB2, "OR A, D"),
	OP(0xB3, "OR A, E"),
	OP(0xB4, "OR A, H"),
	OP(0xB5, "OR A, L"),
	OP(0xB6, "OR A, [HL]"),
	OP(0xB7, "OR A, A"),
	OP(0xB8, "CP A, B"),
	OP(0xB9, "CP A, C"),
	OP(0xBA, "CP A, D"),
	OP(0xBB, "CP A, E"),
	OP(0xBC, "CP A, H"),
	OP(0xBD, "CP A, L"),
	OP(0xBE, "CP A, [HL]"),
	OP(0xBF, "CP A, A"),
	OP(0xC0, "RET NZ"),
	OP(0xC1, "POP BC"),
	OP(0xC2, "JP NZ, a16"),
	OP(0xC3, "JP a16"),
	OP(0xC4, "CALL NZ, a16"),
	OP(0xC5, "PUSH BC"),
	OP(0xC6, "ADD A, n8"),
	OP(0xC7, "RST $00"),
	OP(0xC8, "RET Z"),
	OP(0xC9, "RET"),
	OP(0xCA, "JP Z, a16"),
	OP(0xCB, "PREFIX"),
	OP(0xCC, "CALL Z, a16"),
	OP(0xCD, "CALL a16"),
	OP(0xCE, "ADC A, n8"),
	OP(0xCF, "RST $08"),
	OP(0xD0, "RET NC"),
	OP(0xD1, "POP DE"),
	OP(0xD2, "JP NC, a16"),
	OP(0xD3, "ILLEGAL_D3"),
	OP(0xD4, "CALL NC, a16"),
	OP(0xD5, "PUSH DE"),
	OP(0xD6, "SUB A, n8"),
	OP(0xD7, "RST $10"),
	OP(0xD8, "RET C"),
	OP(0xD9, "RETI"),
	OP(0xDA, "JP C, a16"),
	OP(0xDB, "ILLEGAL_DB"),
	OP(0xDC, "CALL C, a16"),
	OP(0xDD, "ILLEGAL_DD"),
	OP(0xDE, "SBC A, n8"),
	OP(0xDF, "RST $18"),
	OP(0xE0, "LDH [a8], A"),
	OP(0xE1, "POP HL"),
	OP(0xE2, "LDH [C], A"),
	OP(0xE3, "ILLEGAL_E3"),
	OP(0xE4, "ILLEGAL_E4"),
	OP(0xE5, "PUSH HL"),
	OP(0xE6, "AND A, n8"),
	OP(0xE7, "RST $20"),
	OP(0xE8, "ADD SP, e8"),
	OP(0xE9, "JP HL"),
	OP(0xEA, "LD [a16], A"),
	OP(0xEB, "ILLEGAL_EB"),
	OP(0xEC, "ILLEGAL_EC"),
	OP(0xED, "ILLEGAL_ED"),
	OP(0xEE, "XOR A, n8"),
	OP(0xEF, "RST $28"),
	OP(0xF0, "LDH A, [a8]"),
	OP(0xF1, "POP AF"),
	OP(0xF2, "LDH A, [C]"),
	OP(0xF3, "DI"),
	OP(0xF4, "ILLEGAL_F4"),
	OP(0xF5, "PUSH AF"),
	OP(0xF6, "OR A, n8"),
	OP(0xF7, "RST $30"),
	OP(0xF8, "LD HL, SP, e8"),
	OP(0xF9, "LD SP, HL"),
	OP(0xFA, "LD A, [a16]"),
	OP(0xFB, "EI"),
	OP(0xFC, "ILLEGAL_FC"),
	OP(0xFD, "ILLEGAL_FD"),
	OP(0xFE, "CP A, n8"),
	OP(0xFF, "RST $38")
};

constexpr std::array<int, 256> CPU_INSTRUCTION_CYCLES = {
	OP(0x00, 4),
	OP(0x01, 12),
	OP(0x02, 8),
	OP(0x03, 8),
	OP(0x04, 4),
	OP(0x05, 4),
	OP(0x06, 8),
	OP(0x07, 4),
	OP(0x08, 20),
	OP(0x09, 8),
	OP(0x0A, 8),
	OP(0x0B, 8),
	OP(0x0C, 4),
	OP(0x0D, 4),
	OP(0x0E, 8),
	OP(0x0F, 4),
	OP(0x10, 4),
	OP(0x11, 12),
	OP(0x12, 8),
	OP(0x13, 8),
	OP(0x14, 4),
	OP(0x15, 4),
	OP(0x16, 8),
	OP(0x17, 4),
	OP(0x18, 12),
	OP(0x19, 8),
	OP(0x1A, 8),
	OP(0x1B, 8),
	OP(0x1C, 4),
	OP(0x1D, 4),
	OP(0x1E, 8),
	OP(0x1F, 4),
	OP(0x20, 8),
	OP(0x21, 12),
	OP(0x22, 8),
	OP(0x23, 8),
	OP(0x24, 4),
	OP(0x25, 4),
	OP(0x26, 8),
	OP(0x27, 4),
	OP(0x28, 8),
	OP(0x29, 8),
	OP(0x2A, 8),
	OP(0x2B, 8),
	OP(0x2C, 4),
	OP(0x2D, 4),
	OP(0x2E, 8),
	OP(0x2F, 4),
	OP(0x30, 8),
	OP(0x31, 12),
	OP(0x32, 8),
	OP(0x33, 8),
	OP(0x34, 12),
	OP(0x35, 12),
	OP(0x36, 12),
	OP(0x37, 4),
	OP(0x38, 8),
	OP(0x39, 8),
	OP(0x3A, 8),
	OP(0x3B, 8),
	OP(0x3C, 4),
	OP(0x3D, 4),
	OP(0x3E, 8),
	OP(0x3F, 4),
	OP(0x40, 4),
	OP(0x41, 4),
	OP(0x42, 4),
	OP(0x43, 4),
	OP(0x44, 4),
	OP(0x45, 4),
	OP(0x46, 8),
	OP(0x47, 4),
	OP(0x48, 4),
	OP(0x49, 4),
	OP(0x4A, 4),
	OP(0x4B, 4),
	OP(0x4C, 4),
	OP(0x4D, 4),
	OP(0x4E, 8),
	OP(0x4F, 4),
	OP(0x50, 4),
	OP(0x51, 4),
	OP(0x52, 4),
	OP(0x53, 4),
	OP(0x54, 4),
	OP(0x55, 4),
	OP(0x56, 8),
	OP(0x57, 4),
	OP(0x58, 4),
	OP(0x59, 4),
	OP(0x5A, 4),
	OP(0x5B, 4),
	OP(0x5C, 4),
	OP(0x5D, 4),
	OP(0x5E, 8),
	OP(0x5F, 4),
	OP(0x60, 4),
	OP(0x61, 4),
	OP(0x62, 4),
	OP(0x63, 4),
	OP(0x64, 4),
	OP(0x65, 4),
	OP(0x66, 8),
	OP(0x67, 4),
	OP(0x68, 4),
	OP(0x69, 4),
	OP(0x6A, 4),
	OP(0x6B, 4),
	OP(0x6C, 4),
	OP(0x6D, 4),
	OP(0x6E, 8),
	OP(0x6F, 4),
	OP(0x70, 8),
	OP(0x71, 8),
	OP(0x72, 8),
	OP(0x73, 8),
	OP(0x74, 8),
	OP(0x75, 8),
	OP(0x76, 4),
	OP(0x77, 8),
	OP(0x78, 4),
	OP(0x79, 4),
	OP(0x7A, 4),
	OP(0x7B, 4),
	OP(0x7C, 4),
	OP(0x7D, 4),
	OP(0x7E, 8),
	OP(0x7F, 4),
	OP(0x80, 4),
	OP(0x81, 4),
	OP(0x82, 4),
	OP(0x83, 4),
	OP(0x84, 4),
	OP(0x85, 4),
	OP(0x86, 8),
	OP(0x87, 4),
	OP(0x88, 4),
	OP(0x89, 4),
	OP(0x8A, 4),
	OP(0x8B, 4),
	OP(0x8C, 4),
	OP(0x8D, 4),
	OP(0x8E, 8),
	OP(0x8F, 4),
	OP(0x90, 4),
	OP(0x91, 4),
	OP(0x92, 4),
	OP(0x93, 4),
	OP(0x94, 4),
	OP(0x95, 4),
	OP(0x96, 8),
	OP(0x97, 4),
	OP(0x98, 4),
	OP(0x99, 4),
	OP(0x9A, 4),
	OP(0x9B, 4),
	OP(0x9C, 4),
	OP(0x9D, 4),
	OP(0x9E, 8),
	OP(0x9F, 4),
	OP(0xA0, 4),
	OP(0xA1, 4),
	OP(0xA2, 4),
	OP(0xA3, 4),
	OP(0xA4, 4),
	OP(0xA5, 4),
	OP(0xA6, 8),
	OP(0xA7, 4),
	OP(0xA8, 4),
	OP(0xA9, 4),
	OP(0xAA, 4),
	OP(0xAB, 4),
	OP(0xAC, 4),
	OP(0xAD, 4),
	OP(0xAE, 8),
	OP(0xAF, 4),
	OP(0xB0, 4),
	OP(0xB1, 4),
	OP(0xB2, 4),
	OP(0xB3, 4),
	OP(0xB4, 4),
	OP(0xB5, 4),
	OP(0xB6, 8),
	OP(0xB7, 4),
	OP(0xB8, 4),
	OP(0xB9, 4),
	OP(0xBA, 4),
	OP(0xBB, 4),
	OP(0xBC, 4),
	OP(0xBD, 4),
	OP(0xBE, 8),
	OP(0xBF, 4),
	OP(0xC0, 8),
	OP(0xC1, 12),
	OP(0xC2, 12),
	OP(0xC3, 16),
	OP(0xC4, 12),
	OP(0xC5, 16),
	OP(0xC6, 8),
	OP(0xC7, 16),
	OP(0xC8, 8),
	OP(0xC9, 16),
	OP(0xCA, 12),
	OP(0xCB, 4),
	OP(0xCC, 12),
	OP(0xCD, 24),
	OP(0xCE, 8),
	OP(0xCF, 16),
	OP(0xD0, 8),
	OP(0xD1, 12),
	OP(0xD2, 12),
	OP(0xD3, 4),
	OP(0xD4, 12),
	OP(0xD5, 16),
	OP(0xD6, 8),
	OP(0xD7, 16),
	OP(0xD8, 8),
	OP(0xD9, 16),
	OP(0xDA, 12),
	OP(0xDB, 4),
	OP(0xDC, 12),
	OP(0xDD, 4),
	OP(0xDE, 8),
	OP(0xDF, 16),
	OP(0xE0, 12),
	OP(0xE1, 12),
	OP(0xE2, 8),
	OP(0xE3, 4),
	OP(0xE4, 4),
	OP(0xE5, 16),
	OP(0xE6, 8),
	OP(0xE7, 16),
	OP(0xE8, 16),
	OP(0xE9, 4),
	OP(0xEA, 16),
	OP(0xEB, 4),
	OP(0xEC, 4),
	OP(0xED, 4),
	OP(0xEE, 8),
	OP(0xEF, 16),
	OP(0xF0, 12),
	OP(0xF1, 12),
	OP(0xF2, 8),
	OP(0xF3, 4),
	OP(0xF4, 4),
	OP(0xF5, 16),
	OP(0xF6, 8),
	OP(0xF7, 16),
	OP(0xF8, 12),
	OP(0xF9, 8),
	OP(0xFA, 16),
	OP(0xFB, 4),
	OP(0xFC, 4),
	OP(0xFD, 4),
	OP(0xFE, 8),
	OP(0xFF, 16)
};
// clang-format on

int Cpu::executeInstruction(void) {
	uint8_t opcode = m_bus.read8(m_PC++);
	if(opcode != 0xCB) {
		std::cout << "Got instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode)
				  << " at PC = 0x " << std::hex << std::setw(4) << std::setfill('0') << int(m_PC - 1) << ": \""
				  << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"" << std::endl;
	}

	int cycles = CPU_INSTRUCTION_CYCLES.at(opcode);

	switch(opcode) {
	case 0x00: // NOP
		break;
	case 0x01: // LD BC, imm16
		m_C = m_bus.read8(m_PC++);
		m_B = m_bus.read8(m_PC++);
		break;
	case 0x02: // LD [BC], A
		m_bus.write8(BC(), m_A);
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
		doLd(m_B, m_bus.read8(m_PC++));
		break;
	case 0x07: // RLCA
		doRlca();
		break;
	case 0x08: { // LD [imm16], SP
		uint8_t low = m_bus.read8(m_PC++);
		uint8_t high = m_bus.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		m_bus.write16(address, m_SP);
		break;
	}
	case 0x09: // LD HL, BC
		doAdd16ToHL(BC());
		break;
	case 0x0A: // LD A, [BC]
		doLd(m_A, m_bus.read8(BC()));
		break;
	case 0x0B: // DEC BC
		doDec16(m_B, m_C);
		break;
	case 0x0C: // INC C
		doInc(m_C);
		break;
	case 0x0D: // DEC C
		doDec(m_C);
		break;
	case 0x0E: // LD C, imm8
		doLd(m_C, m_bus.read8(m_PC++));
		break;
	case 0x0F: // RRCA
		doRrca();
		break;

	case 0x11: // LD DE, imm16
		m_E = m_bus.read8(m_PC++);
		m_D = m_bus.read8(m_PC++);
		break;
	case 0x12: // LD [DE], A
		m_bus.write8(DE(), m_A);
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
		doLd(m_D, m_bus.read8(m_PC++));
		break;
	case 0x17: // RLA
		doRla();
		break;
	case 0x18: // JR imm8
		doJr();
		break;
	case 0x19: // LD HL, DE
		doAdd16ToHL(DE());
		break;
	case 0x1A: // LD A, [DE]
		doLd(m_A, m_bus.read8(DE()));
		break;
	case 0x1B: // DEC DE
		doDec16(m_D, m_E);
		break;
	case 0x1C: // INC E
		doInc(m_E);
		break;
	case 0x1D: // DEC E
		doDec(m_E);
		break;
	case 0x1E: // LD E, imm8
		doLd(m_E, m_bus.read8(m_PC++));
		break;
	case 0x1F: // RRA
		doRra();
		break;

	case 0x20: // JR NZ, imm8
		if(doJr(!getFlag<Flag::Z>())) { cycles += 4; }
		break;
	case 0x21: // LD HL, imm16
		m_L = m_bus.read8(m_PC++);
		m_H = m_bus.read8(m_PC++);
		break;
	case 0x22: // LD [HL+], A
		m_bus.write8(HL(), m_A);
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
		doLd(m_H, m_bus.read8(m_PC++));
		break;
	case 0x28: // JR Z, imm8
		if(doJr(getFlag<Flag::Z>())) { cycles += 4; }
		break;
	case 0x29: // LD HL, HL
		doAdd16ToHL(HL());
		break;
	case 0x2A: // LD A, [HL+]
		doLd(m_A, m_bus.read8(HL()));
		incHL();
		break;
	case 0x2B: // DEC HL
		doDec16(m_H, m_L);
		break;
	case 0x2C: // INC L
		doInc(m_L);
		break;
	case 0x2D: // DEC L
		doDec(m_L);
		break;
	case 0x2E: // LD L, imm8
		doLd(m_L, m_bus.read8(m_PC++));
		break;
	case 0x2F: // CPL
		m_A = ~m_A;
		setFlag<Flag::N>(true);
		setFlag<Flag::H>(true);
		break;
	case 0x30: // JR NC, imm8
		if(doJr(!getFlag<Flag::C>())) { cycles += 4; }
		break;
	case 0x31: { // LD SP, imm16
		uint8_t low = m_bus.read8(m_PC++);
		uint8_t high = m_bus.read8(m_PC++);
		m_SP = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		break;
	}
	case 0x32: // LD [HL-], A
		m_bus.write8(HL(), m_A);
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
		m_bus.write8(HL(), m_bus.read8(m_PC++));
		break;
	case 0x38: // JR C, imm8
		if(doJr(getFlag<Flag::C>())) { cycles += 4; }
		break;
	case 0x39: // LD HL, SP
		doAdd16ToHL(SP());
		break;
	case 0x3A: // LD A, [HL-]
		doLd(m_A, m_bus.read8(HL()));
		decHL();
		break;
	case 0x3B: // DEC SP
		doDec16(m_SP);
		break;
	case 0x3C: // INC A
		doInc(m_A);
		break;
	case 0x3D: // DEC A
		doDec(m_A);
		break;
	case 0x3E: // LD A, imm8
		doLd(m_A, m_bus.read8(m_PC++));
		break;
	case 0x3F: // CCF
		setFlag<Flag::C>(!getFlag<Flag::C>());
		setFlag<Flag::N>(false);
		setFlag<Flag::H>(false);
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
		doLd(m_B, m_bus.read8(HL()));
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
		doLd(m_C, m_bus.read8(HL()));
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
		doLd(m_D, m_bus.read8(HL()));
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
		doLd(m_E, m_bus.read8(HL()));
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
		doLd(m_H, m_bus.read8(HL()));
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
		doLd(m_L, m_bus.read8(HL()));
		break;
	case 0x6F: // LD L, A
		doLd(m_L, m_A);
		break;
	case 0x70: // LD [HL], B
		m_bus.write8(HL(), m_B);
		break;
	case 0x71: // LD [HL], C
		m_bus.write8(HL(), m_C);
		break;
	case 0x72: // LD [HL], D
		m_bus.write8(HL(), m_D);
		break;
	case 0x73: // LD [HL], E
		m_bus.write8(HL(), m_E);
		break;
	case 0x74: // LD [HL], H
		m_bus.write8(HL(), m_H);
		break;
	case 0x75: // LD [HL], L
		m_bus.write8(HL(), m_L);
		break;

	case 0x77: // LD [HL], A
		m_bus.write8(HL(), m_A);
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
		doLd(m_A, m_bus.read8(HL()));
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
		doAdd(m_A, m_bus.read8(HL()));
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
		doAdc(m_A, m_bus.read8(HL()));
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
		doSub(m_A, m_bus.read8(HL()));
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
		doSbc(m_A, m_bus.read8(HL()));
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
		doAnd(m_A, m_bus.read8(HL()));
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
		doXor(m_A, m_bus.read8(HL()));
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
		doOr(m_A, m_bus.read8(HL()));
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
		doCp(m_A, m_bus.read8(HL()));
		break;
	case 0xBF: // CP A, A
		doCp(m_A, m_A);
		break;
	case 0xC0: // RET NZ
		if(doRet(!getFlag<Flag::Z>())) { cycles += 12; }
		break;
	case 0xC1: // POP BC
		doPop(m_B, m_C);
		break;
	case 0xC2: // JP NZ, imm16
		if(doJp(!getFlag<Flag::Z>())) { cycles += 4; }
		break;
	case 0xC3: // JP imm16
		doJp();
		break;
	case 0xC4: // CALL NZ, imm16
		if(doCall(!getFlag<Flag::Z>())) { cycles += 12; }
		break;
	case 0xC5: // PUSH BC
		doPush(BC());
		break;
	case 0xC6: // ADD A, imm8
		doAdd(m_A, m_bus.read8(m_PC++));
		break;
	case 0xC7: // RST 0x00
		doPush(m_PC);
		m_PC = 0x0000;
		break;
	case 0xC8: // RET Z
		if(doRet(getFlag<Flag::Z>())) { cycles += 12; }
		break;
	case 0xC9: // RET
		doRet();
		break;
	case 0xCA: // JP Z, imm16
		if(doJp(getFlag<Flag::Z>())) { cycles += 4; }
		break;
	case 0xCB: // PREFIX
		executeCbInstruction();
		break;
	case 0xCC: // CALL Z, imm16
		if(doCall(getFlag<Flag::Z>())) { cycles += 12; }
		break;
	case 0xCD: // CALL imm16
		doCall();
		break;
	case 0xCE: // ADC A, imm8
		doAdc(m_A, m_bus.read8(m_PC++));
		break;
	case 0xCF: // RST 0x08
		doPush(m_PC);
		m_PC = 0x08;
		break;
	case 0xD0: // RET NC
		if(doRet(!getFlag<Flag::C>())) { cycles += 12; }
		break;
	case 0xD1: // POP DE
		doPop(m_D, m_E);
		break;
	case 0xD2: // JP NC, imm16
		if(doJp(!getFlag<Flag::C>())) { cycles += 4; }
		break;
	case 0xD4: // CALL NC, imm16
		if(doCall(!getFlag<Flag::C>())) { cycles += 12; }
		break;
	case 0xD5: // PUSH DE
		doPush(DE());
		break;
	case 0xD6: // SUB A, imm8
		doSub(m_A, m_bus.read8(m_PC++));
		break;
	case 0xD7: // RST 0x10
		doPush(m_PC);
		m_PC = 0x10;
		break;
	case 0xD8: // RET C
		if(doRet(getFlag<Flag::C>())) { cycles += 12; }
		break;
	case 0xD9: // RETI
		doPop(m_PC);
		m_IME = 1;
		break;
	case 0xDA: // JP C, imm16
		if(doJp(getFlag<Flag::C>())) { cycles += 4; }
		break;
	case 0xDC: // CALL C, imm16
		if(doCall(getFlag<Flag::C>())) { cycles += 12; }
		break;
	case 0xDE: // SBC A, imm8
		doSbc(m_A, m_bus.read8(m_PC++));
		break;
	case 0xDF: // RST 0x18
		doPush(m_PC);
		m_PC = 0x18;
		break;
	case 0xE0: { // LDH [imm8], A
		uint8_t low = m_bus.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		m_bus.write8(address, m_A);
		break;
	}
	case 0xE1: // POP HL
		doPop(m_H, m_L);
		break;
	case 0xE2: { // LDH [C], A
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		m_bus.write8(address, m_A);
		break;
	}
	case 0xE5: // PUSH HL
		doPush(HL());
		break;
	case 0xE6: // AND A, imm8
		doAnd(m_A, m_bus.read8(m_PC++));
		break;
	case 0xE7: // RST 0x20
		doPush(m_PC);
		m_PC = 0x20;
		break;
	case 0xE8: { // ADD SP, e8
		uint16_t imm8 = static_cast<uint16_t>(m_bus.read8(m_PC++));
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
		uint8_t low = m_bus.read8(m_PC++);
		uint8_t high = m_bus.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		m_bus.write8(address, m_A);
		break;
	}
	case 0xEE: // XOR A, imm8
		doXor(m_A, m_bus.read8(m_PC++));
		break;
	case 0xEF: // RST 0x28
		doPush(m_PC);
		m_PC = 0x28;
		break;
	case 0xF0: { // LDH A, [imm8]
		uint8_t low = m_bus.read8(m_PC++);
		uint16_t address = 0xFF00 | static_cast<uint16_t>(low);
		doLd(m_A, m_bus.read8(address));
		break;
	}
	case 0xF1: // POP AF
		doPop(m_A, m_F);
		break;
	case 0xF2: { // LDH A, [C]
		uint16_t address = 0xFF00 | static_cast<uint16_t>(m_C);
		doLd(m_A, m_bus.read8(address));
		break;
	}
	case 0xF3: // DI
		m_IME = false;
		break;
	case 0xF5: // PUSH AF
		doPush(AF());
		break;
	case 0xF6: // OR A, imm8
		doOr(m_A, m_bus.read8(m_PC++));
		break;
	case 0xF7: // RST 0x30
		doPush(m_PC);
		m_PC = 0x30;
		break;
	case 0xF8: { // LD HL, SP+e8
		uint16_t imm8 = static_cast<uint16_t>(m_bus.read8(m_PC++));
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
		uint8_t low = m_bus.read8(m_PC++);
		uint8_t high = m_bus.read8(m_PC++);
		uint16_t address = (static_cast<uint16_t>(high) << 8) | static_cast<uint16_t>(low);
		doLd(m_A, m_bus.read8(address));
		break;
	}
	case 0xFB: // EI
		m_IME = true;
		break;
	case 0xFE: // CP A, imm8
		doCp(m_A, m_bus.read8(m_PC++));
		break;
	case 0xFF: // RST 0x38
		doPush(m_PC);
		m_PC = 0x38;
		break;
	default: {
		std::stringstream ss;
		ss << "Unhandled instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode) << " at PC=0x"
		   << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
		   << CPU_INSTRUCTION_MNEMONICS.at(opcode) << "\"";

		throw std::runtime_error(ss.str());
	}
	}

	return cycles;
}
