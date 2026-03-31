#include <array>
#include "config.hpp"
#include "cpu.hpp"
#include <cstdint>
#include <iomanip>
#include <ios>
#include <iostream>
#include <sstream>

// Useful to visually see the OPCODE-MNEMONIC/CYCLE
#define OP(op, data) data

// clang-format off
constexpr std::array<const char *, 256> CPU_CB_INSTRUCTION_MNEMONICS = {
    OP(0x00, "RLC B"),
    OP(0x01, "RLC C"),
    OP(0x02, "RLC D"),
    OP(0x03, "RLC E"),
    OP(0x04, "RLC H"),
    OP(0x05, "RLC L"),
    OP(0x06, "RLC [HL]"),
    OP(0x07, "RLC A"),
    OP(0x08, "RRC B"),
    OP(0x09, "RRC C"),
    OP(0x0A, "RRC D"),
    OP(0x0B, "RRC E"),
    OP(0x0C, "RRC H"),
    OP(0x0D, "RRC L"),
    OP(0x0E, "RRC [HL]"),
    OP(0x0F, "RRC A"),
    OP(0x10, "RL B"),
    OP(0x11, "RL C"),
    OP(0x12, "RL D"),
    OP(0x13, "RL E"),
    OP(0x14, "RL H"),
    OP(0x15, "RL L"),
    OP(0x16, "RL [HL]"),
    OP(0x17, "RL A"),
    OP(0x18, "RR B"),
    OP(0x19, "RR C"),
    OP(0x1A, "RR D"),
    OP(0x1B, "RR E"),
    OP(0x1C, "RR H"),
    OP(0x1D, "RR L"),
    OP(0x1E, "RR [HL]"),
    OP(0x1F, "RR A"),
    OP(0x20, "SLA B"),
    OP(0x21, "SLA C"),
    OP(0x22, "SLA D"),
    OP(0x23, "SLA E"),
    OP(0x24, "SLA H"),
    OP(0x25, "SLA L"),
    OP(0x26, "SLA [HL]"),
    OP(0x27, "SLA A"),
    OP(0x28, "SRA B"),
    OP(0x29, "SRA C"),
    OP(0x2A, "SRA D"),
    OP(0x2B, "SRA E"),
    OP(0x2C, "SRA H"),
    OP(0x2D, "SRA L"),
    OP(0x2E, "SRA [HL]"),
    OP(0x2F, "SRA A"),
    OP(0x30, "SWAP B"),
    OP(0x31, "SWAP C"),
    OP(0x32, "SWAP D"),
    OP(0x33, "SWAP E"),
    OP(0x34, "SWAP H"),
    OP(0x35, "SWAP L"),
    OP(0x36, "SWAP [HL]"),
    OP(0x37, "SWAP A"),
    OP(0x38, "SRL B"),
    OP(0x39, "SRL C"),
    OP(0x3A, "SRL D"),
    OP(0x3B, "SRL E"),
    OP(0x3C, "SRL H"),
    OP(0x3D, "SRL L"),
    OP(0x3E, "SRL [HL]"),
    OP(0x3F, "SRL A"),
    OP(0x40, "BIT 0, B"),
    OP(0x41, "BIT 0, C"),
    OP(0x42, "BIT 0, D"),
    OP(0x43, "BIT 0, E"),
    OP(0x44, "BIT 0, H"),
    OP(0x45, "BIT 0, L"),
    OP(0x46, "BIT 0, [HL]"),
    OP(0x47, "BIT 0, A"),
    OP(0x48, "BIT 1, B"),
    OP(0x49, "BIT 1, C"),
    OP(0x4A, "BIT 1, D"),
    OP(0x4B, "BIT 1, E"),
    OP(0x4C, "BIT 1, H"),
    OP(0x4D, "BIT 1, L"),
    OP(0x4E, "BIT 1, [HL]"),
    OP(0x4F, "BIT 1, A"),
    OP(0x50, "BIT 2, B"),
    OP(0x51, "BIT 2, C"),
    OP(0x52, "BIT 2, D"),
    OP(0x53, "BIT 2, E"),
    OP(0x54, "BIT 2, H"),
    OP(0x55, "BIT 2, L"),
    OP(0x56, "BIT 2, [HL]"),
    OP(0x57, "BIT 2, A"),
    OP(0x58, "BIT 3, B"),
    OP(0x59, "BIT 3, C"),
    OP(0x5A, "BIT 3, D"),
    OP(0x5B, "BIT 3, E"),
    OP(0x5C, "BIT 3, H"),
    OP(0x5D, "BIT 3, L"),
    OP(0x5E, "BIT 3, [HL]"),
    OP(0x5F, "BIT 3, A"),
    OP(0x60, "BIT 4, B"),
    OP(0x61, "BIT 4, C"),
    OP(0x62, "BIT 4, D"),
    OP(0x63, "BIT 4, E"),
    OP(0x64, "BIT 4, H"),
    OP(0x65, "BIT 4, L"),
    OP(0x66, "BIT 4, [HL]"),
    OP(0x67, "BIT 4, A"),
    OP(0x68, "BIT 5, B"),
    OP(0x69, "BIT 5, C"),
    OP(0x6A, "BIT 5, D"),
    OP(0x6B, "BIT 5, E"),
    OP(0x6C, "BIT 5, H"),
    OP(0x6D, "BIT 5, L"),
    OP(0x6E, "BIT 5, [HL]"),
    OP(0x6F, "BIT 5, A"),
    OP(0x70, "BIT 6, B"),
    OP(0x71, "BIT 6, C"),
    OP(0x72, "BIT 6, D"),
    OP(0x73, "BIT 6, E"),
    OP(0x74, "BIT 6, H"),
    OP(0x75, "BIT 6, L"),
    OP(0x76, "BIT 6, [HL]"),
    OP(0x77, "BIT 6, A"),
    OP(0x78, "BIT 7, B"),
    OP(0x79, "BIT 7, C"),
    OP(0x7A, "BIT 7, D"),
    OP(0x7B, "BIT 7, E"),
    OP(0x7C, "BIT 7, H"),
    OP(0x7D, "BIT 7, L"),
    OP(0x7E, "BIT 7, [HL]"),
    OP(0x7F, "BIT 7, A"),
    OP(0x80, "RES 0, B"),
    OP(0x81, "RES 0, C"),
    OP(0x82, "RES 0, D"),
    OP(0x83, "RES 0, E"),
    OP(0x84, "RES 0, H"),
    OP(0x85, "RES 0, L"),
    OP(0x86, "RES 0, [HL]"),
    OP(0x87, "RES 0, A"),
    OP(0x88, "RES 1, B"),
    OP(0x89, "RES 1, C"),
    OP(0x8A, "RES 1, D"),
    OP(0x8B, "RES 1, E"),
    OP(0x8C, "RES 1, H"),
    OP(0x8D, "RES 1, L"),
    OP(0x8E, "RES 1, [HL]"),
    OP(0x8F, "RES 1, A"),
    OP(0x90, "RES 2, B"),
    OP(0x91, "RES 2, C"),
    OP(0x92, "RES 2, D"),
    OP(0x93, "RES 2, E"),
    OP(0x94, "RES 2, H"),
    OP(0x95, "RES 2, L"),
    OP(0x96, "RES 2, [HL]"),
    OP(0x97, "RES 2, A"),
    OP(0x98, "RES 3, B"),
    OP(0x99, "RES 3, C"),
    OP(0x9A, "RES 3, D"),
    OP(0x9B, "RES 3, E"),
    OP(0x9C, "RES 3, H"),
    OP(0x9D, "RES 3, L"),
    OP(0x9E, "RES 3, [HL]"),
    OP(0x9F, "RES 3, A"),
    OP(0xA0, "RES 4, B"),
    OP(0xA1, "RES 4, C"),
    OP(0xA2, "RES 4, D"),
    OP(0xA3, "RES 4, E"),
    OP(0xA4, "RES 4, H"),
    OP(0xA5, "RES 4, L"),
    OP(0xA6, "RES 4, [HL]"),
    OP(0xA7, "RES 4, A"),
    OP(0xA8, "RES 5, B"),
    OP(0xA9, "RES 5, C"),
    OP(0xAA, "RES 5, D"),
    OP(0xAB, "RES 5, E"),
    OP(0xAC, "RES 5, H"),
    OP(0xAD, "RES 5, L"),
    OP(0xAE, "RES 5, [HL]"),
    OP(0xAF, "RES 5, A"),
    OP(0xB0, "RES 6, B"),
    OP(0xB1, "RES 6, C"),
    OP(0xB2, "RES 6, D"),
    OP(0xB3, "RES 6, E"),
    OP(0xB4, "RES 6, H"),
    OP(0xB5, "RES 6, L"),
    OP(0xB6, "RES 6, [HL]"),
    OP(0xB7, "RES 6, A"),
    OP(0xB8, "RES 7, B"),
    OP(0xB9, "RES 7, C"),
    OP(0xBA, "RES 7, D"),
    OP(0xBB, "RES 7, E"),
    OP(0xBC, "RES 7, H"),
    OP(0xBD, "RES 7, L"),
    OP(0xBE, "RES 7, [HL]"),
    OP(0xBF, "RES 7, A"),
    OP(0xC0, "SET 0, B"),
    OP(0xC1, "SET 0, C"),
    OP(0xC2, "SET 0, D"),
    OP(0xC3, "SET 0, E"),
    OP(0xC4, "SET 0, H"),
    OP(0xC5, "SET 0, L"),
    OP(0xC6, "SET 0, [HL]"),
    OP(0xC7, "SET 0, A"),
    OP(0xC8, "SET 1, B"),
    OP(0xC9, "SET 1, C"),
    OP(0xCA, "SET 1, D"),
    OP(0xCB, "SET 1, E"),
    OP(0xCC, "SET 1, H"),
    OP(0xCD, "SET 1, L"),
    OP(0xCE, "SET 1, [HL]"),
    OP(0xCF, "SET 1, A"),
    OP(0xD0, "SET 2, B"),
    OP(0xD1, "SET 2, C"),
    OP(0xD2, "SET 2, D"),
    OP(0xD3, "SET 2, E"),
    OP(0xD4, "SET 2, H"),
    OP(0xD5, "SET 2, L"),
    OP(0xD6, "SET 2, [HL]"),
    OP(0xD7, "SET 2, A"),
    OP(0xD8, "SET 3, B"),
    OP(0xD9, "SET 3, C"),
    OP(0xDA, "SET 3, D"),
    OP(0xDB, "SET 3, E"),
    OP(0xDC, "SET 3, H"),
    OP(0xDD, "SET 3, L"),
    OP(0xDE, "SET 3, [HL]"),
    OP(0xDF, "SET 3, A"),
    OP(0xE0, "SET 4, B"),
    OP(0xE1, "SET 4, C"),
    OP(0xE2, "SET 4, D"),
    OP(0xE3, "SET 4, E"),
    OP(0xE4, "SET 4, H"),
    OP(0xE5, "SET 4, L"),
    OP(0xE6, "SET 4, [HL]"),
    OP(0xE7, "SET 4, A"),
    OP(0xE8, "SET 5, B"),
    OP(0xE9, "SET 5, C"),
    OP(0xEA, "SET 5, D"),
    OP(0xEB, "SET 5, E"),
    OP(0xEC, "SET 5, H"),
    OP(0xED, "SET 5, L"),
    OP(0xEE, "SET 5, [HL]"),
    OP(0xEF, "SET 5, A"),
    OP(0xF0, "SET 6, B"),
    OP(0xF1, "SET 6, C"),
    OP(0xF2, "SET 6, D"),
    OP(0xF3, "SET 6, E"),
    OP(0xF4, "SET 6, H"),
    OP(0xF5, "SET 6, L"),
    OP(0xF6, "SET 6, [HL]"),
    OP(0xF7, "SET 6, A"),
    OP(0xF8, "SET 7, B"),
    OP(0xF9, "SET 7, C"),
    OP(0xFA, "SET 7, D"),
    OP(0xFB, "SET 7, E"),
    OP(0xFC, "SET 7, H"),
    OP(0xFD, "SET 7, L"),
    OP(0xFE, "SET 7, [HL]"),
    OP(0xFF, "SET 7, A")
};

constexpr std::array<int, 256> CPU_CB_INSTRUCTION_CYCLES = {
    OP(0x00, 8),
    OP(0x01, 8),
    OP(0x02, 8),
    OP(0x03, 8),
    OP(0x04, 8),
    OP(0x05, 8),
    OP(0x06, 16),
    OP(0x07, 8),
    OP(0x08, 8),
    OP(0x09, 8),
    OP(0x0A, 8),
    OP(0x0B, 8),
    OP(0x0C, 8),
    OP(0x0D, 8),
    OP(0x0E, 16),
    OP(0x0F, 8),
    OP(0x10, 8),
    OP(0x11, 8),
    OP(0x12, 8),
    OP(0x13, 8),
    OP(0x14, 8),
    OP(0x15, 8),
    OP(0x16, 16),
    OP(0x17, 8),
    OP(0x18, 8),
    OP(0x19, 8),
    OP(0x1A, 8),
    OP(0x1B, 8),
    OP(0x1C, 8),
    OP(0x1D, 8),
    OP(0x1E, 16),
    OP(0x1F, 8),
    OP(0x20, 8),
    OP(0x21, 8),
    OP(0x22, 8),
    OP(0x23, 8),
    OP(0x24, 8),
    OP(0x25, 8),
    OP(0x26, 16),
    OP(0x27, 8),
    OP(0x28, 8),
    OP(0x29, 8),
    OP(0x2A, 8),
    OP(0x2B, 8),
    OP(0x2C, 8),
    OP(0x2D, 8),
    OP(0x2E, 16),
    OP(0x2F, 8),
    OP(0x30, 8),
    OP(0x31, 8),
    OP(0x32, 8),
    OP(0x33, 8),
    OP(0x34, 8),
    OP(0x35, 8),
    OP(0x36, 16),
    OP(0x37, 8),
    OP(0x38, 8),
    OP(0x39, 8),
    OP(0x3A, 8),
    OP(0x3B, 8),
    OP(0x3C, 8),
    OP(0x3D, 8),
    OP(0x3E, 16),
    OP(0x3F, 8),
    OP(0x40, 8),
    OP(0x41, 8),
    OP(0x42, 8),
    OP(0x43, 8),
    OP(0x44, 8),
    OP(0x45, 8),
    OP(0x46, 12),
    OP(0x47, 8),
    OP(0x48, 8),
    OP(0x49, 8),
    OP(0x4A, 8),
    OP(0x4B, 8),
    OP(0x4C, 8),
    OP(0x4D, 8),
    OP(0x4E, 12),
    OP(0x4F, 8),
    OP(0x50, 8),
    OP(0x51, 8),
    OP(0x52, 8),
    OP(0x53, 8),
    OP(0x54, 8),
    OP(0x55, 8),
    OP(0x56, 12),
    OP(0x57, 8),
    OP(0x58, 8),
    OP(0x59, 8),
    OP(0x5A, 8),
    OP(0x5B, 8),
    OP(0x5C, 8),
    OP(0x5D, 8),
    OP(0x5E, 12),
    OP(0x5F, 8),
    OP(0x60, 8),
    OP(0x61, 8),
    OP(0x62, 8),
    OP(0x63, 8),
    OP(0x64, 8),
    OP(0x65, 8),
    OP(0x66, 12),
    OP(0x67, 8),
    OP(0x68, 8),
    OP(0x69, 8),
    OP(0x6A, 8),
    OP(0x6B, 8),
    OP(0x6C, 8),
    OP(0x6D, 8),
    OP(0x6E, 12),
    OP(0x6F, 8),
    OP(0x70, 8),
    OP(0x71, 8),
    OP(0x72, 8),
    OP(0x73, 8),
    OP(0x74, 8),
    OP(0x75, 8),
    OP(0x76, 12),
    OP(0x77, 8),
    OP(0x78, 8),
    OP(0x79, 8),
    OP(0x7A, 8),
    OP(0x7B, 8),
    OP(0x7C, 8),
    OP(0x7D, 8),
    OP(0x7E, 12),
    OP(0x7F, 8),
    OP(0x80, 8),
    OP(0x81, 8),
    OP(0x82, 8),
    OP(0x83, 8),
    OP(0x84, 8),
    OP(0x85, 8),
    OP(0x86, 16),
    OP(0x87, 8),
    OP(0x88, 8),
    OP(0x89, 8),
    OP(0x8A, 8),
    OP(0x8B, 8),
    OP(0x8C, 8),
    OP(0x8D, 8),
    OP(0x8E, 16),
    OP(0x8F, 8),
    OP(0x90, 8),
    OP(0x91, 8),
    OP(0x92, 8),
    OP(0x93, 8),
    OP(0x94, 8),
    OP(0x95, 8),
    OP(0x96, 16),
    OP(0x97, 8),
    OP(0x98, 8),
    OP(0x99, 8),
    OP(0x9A, 8),
    OP(0x9B, 8),
    OP(0x9C, 8),
    OP(0x9D, 8),
    OP(0x9E, 16),
    OP(0x9F, 8),
    OP(0xA0, 8),
    OP(0xA1, 8),
    OP(0xA2, 8),
    OP(0xA3, 8),
    OP(0xA4, 8),
    OP(0xA5, 8),
    OP(0xA6, 16),
    OP(0xA7, 8),
    OP(0xA8, 8),
    OP(0xA9, 8),
    OP(0xAA, 8),
    OP(0xAB, 8),
    OP(0xAC, 8),
    OP(0xAD, 8),
    OP(0xAE, 16),
    OP(0xAF, 8),
    OP(0xB0, 8),
    OP(0xB1, 8),
    OP(0xB2, 8),
    OP(0xB3, 8),
    OP(0xB4, 8),
    OP(0xB5, 8),
    OP(0xB6, 16),
    OP(0xB7, 8),
    OP(0xB8, 8),
    OP(0xB9, 8),
    OP(0xBA, 8),
    OP(0xBB, 8),
    OP(0xBC, 8),
    OP(0xBD, 8),
    OP(0xBE, 16),
    OP(0xBF, 8),
    OP(0xC0, 8),
    OP(0xC1, 8),
    OP(0xC2, 8),
    OP(0xC3, 8),
    OP(0xC4, 8),
    OP(0xC5, 8),
    OP(0xC6, 16),
    OP(0xC7, 8),
    OP(0xC8, 8),
    OP(0xC9, 8),
    OP(0xCA, 8),
    OP(0xCB, 8),
    OP(0xCC, 8),
    OP(0xCD, 8),
    OP(0xCE, 16),
    OP(0xCF, 8),
    OP(0xD0, 8),
    OP(0xD1, 8),
    OP(0xD2, 8),
    OP(0xD3, 8),
    OP(0xD4, 8),
    OP(0xD5, 8),
    OP(0xD6, 16),
    OP(0xD7, 8),
    OP(0xD8, 8),
    OP(0xD9, 8),
    OP(0xDA, 8),
    OP(0xDB, 8),
    OP(0xDC, 8),
    OP(0xDD, 8),
    OP(0xDE, 16),
    OP(0xDF, 8),
    OP(0xE0, 8),
    OP(0xE1, 8),
    OP(0xE2, 8),
    OP(0xE3, 8),
    OP(0xE4, 8),
    OP(0xE5, 8),
    OP(0xE6, 16),
    OP(0xE7, 8),
    OP(0xE8, 8),
    OP(0xE9, 8),
    OP(0xEA, 8),
    OP(0xEB, 8),
    OP(0xEC, 8),
    OP(0xED, 8),
    OP(0xEE, 16),
    OP(0xEF, 8),
    OP(0xF0, 8),
    OP(0xF1, 8),
    OP(0xF2, 8),
    OP(0xF3, 8),
    OP(0xF4, 8),
    OP(0xF5, 8),
    OP(0xF6, 16),
    OP(0xF7, 8),
    OP(0xF8, 8),
    OP(0xF9, 8),
    OP(0xFA, 8),
    OP(0xFB, 8),
    OP(0xFC, 8),
    OP(0xFD, 8),
    OP(0xFE, 16),
    OP(0xFF, 8)
};
// clang-format on

int Cpu::executeCbInstruction(void) {
	uint8_t opcode = m_bus.read8(m_PC++);
	if(Config::debugOutput) {
		std::cout << "Got prefixed instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode)
				  << " at PC=0x" << std::hex << std::setw(4) << std::setfill('0') << int(m_PC - 2) << ": \""
				  << CPU_CB_INSTRUCTION_MNEMONICS.at(opcode) << "\"" << std::endl;
	}

	switch(opcode) {
	case 0x00: // RLC B
		doRlc(m_B);
		break;
	case 0x01: // RLC C
		doRlc(m_C);
		break;
	case 0x02: // RLC D
		doRlc(m_D);
		break;
	case 0x03: // RLC E
		doRlc(m_E);
		break;
	case 0x04: // RLC H
		doRlc(m_H);
		break;
	case 0x05: // RLC L
		doRlc(m_L);
		break;
	case 0x06: // RLC [HL]
		doRlc(HL());
		break;
	case 0x07: // RLC A
		doRlc(m_A);
		break;
	case 0x08: // RRC B
		doRrc(m_B);
		break;
	case 0x09: // RRC C
		doRrc(m_C);
		break;
	case 0x0A: // RRC D
		doRrc(m_D);
		break;
	case 0x0B: // RRC E
		doRrc(m_E);
		break;
	case 0x0C: // RRC H
		doRrc(m_H);
		break;
	case 0x0D: // RRC L
		doRrc(m_L);
		break;
	case 0x0E: // RRC [HL]
		doRrc(HL());
		break;
	case 0x0F: // RRC A
		doRrc(m_A);
		break;
	case 0x10: // RL B
		doRl(m_B);
		break;
	case 0x11: // RL C
		doRl(m_C);
		break;
	case 0x12: // RL D
		doRl(m_D);
		break;
	case 0x13: // RL E
		doRl(m_E);
		break;
	case 0x14: // RL H
		doRl(m_H);
		break;
	case 0x15: // RL L
		doRl(m_L);
		break;
	case 0x16: // RL [HL]
		doRl(HL());
		break;
	case 0x17: // RL A
		doRl(m_A);
		break;
	case 0x18: // RR B
		doRr(m_B);
		break;
	case 0x19: // RR C
		doRr(m_C);
		break;
	case 0x1A: // RR D
		doRr(m_D);
		break;
	case 0x1B: // RR E
		doRr(m_E);
		break;
	case 0x1C: // RR H
		doRr(m_H);
		break;
	case 0x1D: // RR L
		doRr(m_L);
		break;
	case 0x1E: // RR [HL]
		doRr(HL());
		break;
	case 0x1F: // RR A
		doRr(m_A);
		break;
	case 0x20: // SLA B
		doSla(m_B);
		break;
	case 0x21: // SLA C
		doSla(m_C);
		break;
	case 0x22: // SLA D
		doSla(m_D);
		break;
	case 0x23: // SLA E
		doSla(m_E);
		break;
	case 0x24: // SLA H
		doSla(m_H);
		break;
	case 0x25: // SLA L
		doSla(m_L);
		break;
	case 0x26: // SLA [HL]
		doSla(HL());
		break;
	case 0x27: // SLA A
		doSla(m_A);
		break;
	case 0x28: // SRA B
		doSra(m_B);
		break;
	case 0x29: // SRA C
		doSra(m_C);
		break;
	case 0x2A: // SRA D
		doSra(m_D);
		break;
	case 0x2B: // SRA E
		doSra(m_E);
		break;
	case 0x2C: // SRA H
		doSra(m_H);
		break;
	case 0x2D: // SRA L
		doSra(m_L);
		break;
	case 0x2E: // SRA [HL]
		doSra(HL());
		break;
	case 0x2F: // SRA A
		doSra(m_A);
		break;
	case 0x30: // SWAP B
		doSwap(m_B);
		break;
	case 0x31: // SWAP C
		doSwap(m_C);
		break;
	case 0x32: // SWAP D
		doSwap(m_D);
		break;
	case 0x33: // SWAP E
		doSwap(m_E);
		break;
	case 0x34: // SWAP H
		doSwap(m_H);
		break;
	case 0x35: // SWAP L
		doSwap(m_L);
		break;
	case 0x36: // SWAP [HL]
		doSwap(HL());
		break;
	case 0x37: // SWAP A
		doSwap(m_A);
		break;
	case 0x38: // SRL B
		doSrl(m_B);
		break;
	case 0x39: // SRL C
		doSrl(m_C);
		break;
	case 0x3A: // SRL D
		doSrl(m_D);
		break;
	case 0x3B: // SRL E
		doSrl(m_E);
		break;
	case 0x3C: // SRL H
		doSrl(m_H);
		break;
	case 0x3D: // SRL L
		doSrl(m_L);
		break;
	case 0x3E: // SRL [HL]
		doSrl(HL());
		break;
	case 0x3F: // SRL A
		doSrl(m_A);
		break;
	case 0x40: // BIT 0, B
		doBit(m_B, 0);
		break;
	case 0x41: // BIT 0, C
		doBit(m_C, 0);
		break;
	case 0x42: // BIT 0, D
		doBit(m_D, 0);
		break;
	case 0x43: // BIT 0, E
		doBit(m_E, 0);
		break;
	case 0x44: // BIT 0, H
		doBit(m_H, 0);
		break;
	case 0x45: // BIT 0, L
		doBit(m_L, 0);
		break;
	case 0x46: // BIT 0, [HL]
		doBit(HL(), 0);
		break;
	case 0x47: // BIT 0, A
		doBit(m_A, 0);
		break;
	case 0x48: // BIT 1, B
		doBit(m_B, 1);
		break;
	case 0x49: // BIT 1, C
		doBit(m_C, 1);
		break;
	case 0x4A: // BIT 1, D
		doBit(m_D, 1);
		break;
	case 0x4B: // BIT 1, E
		doBit(m_E, 1);
		break;
	case 0x4C: // BIT 1, H
		doBit(m_H, 1);
		break;
	case 0x4D: // BIT 1, L
		doBit(m_L, 1);
		break;
	case 0x4E: // BIT 1, [HL]
		doBit(HL(), 1);
		break;
	case 0x4F: // BIT 1, A
		doBit(m_A, 1);
		break;
	case 0x50: // BIT 2, B
		doBit(m_B, 2);
		break;
	case 0x51: // BIT 2, C
		doBit(m_C, 2);
		break;
	case 0x52: // BIT 2, D
		doBit(m_D, 2);
		break;
	case 0x53: // BIT 2, E
		doBit(m_E, 2);
		break;
	case 0x54: // BIT 2, H
		doBit(m_H, 2);
		break;
	case 0x55: // BIT 2, L
		doBit(m_L, 2);
		break;
	case 0x56: // BIT 2, [HL]
		doBit(HL(), 2);
		break;
	case 0x57: // BIT 2, A
		doBit(m_A, 2);
		break;
	case 0x58: // BIT 3, B
		doBit(m_B, 3);
		break;
	case 0x59: // BIT 3, C
		doBit(m_C, 3);
		break;
	case 0x5A: // BIT 3, D
		doBit(m_D, 3);
		break;
	case 0x5B: // BIT 3, E
		doBit(m_E, 3);
		break;
	case 0x5C: // BIT 3, H
		doBit(m_H, 3);
		break;
	case 0x5D: // BIT 3, L
		doBit(m_L, 3);
		break;
	case 0x5E: // BIT 3, [HL]
		doBit(HL(), 3);
		break;
	case 0x5F: // BIT 3, A
		doBit(m_A, 3);
		break;
	case 0x60: // BIT 4, B
		doBit(m_B, 4);
		break;
	case 0x61: // BIT 4, C
		doBit(m_C, 4);
		break;
	case 0x62: // BIT 4, D
		doBit(m_D, 4);
		break;
	case 0x63: // BIT 4, E
		doBit(m_E, 4);
		break;
	case 0x64: // BIT 4, H
		doBit(m_H, 4);
		break;
	case 0x65: // BIT 4, L
		doBit(m_L, 4);
		break;
	case 0x66: // BIT 4, [HL]
		doBit(HL(), 4);
		break;
	case 0x67: // BIT 4, A
		doBit(m_A, 4);
		break;
	case 0x68: // BIT 5, B
		doBit(m_B, 5);
		break;
	case 0x69: // BIT 5, C
		doBit(m_C, 5);
		break;
	case 0x6A: // BIT 5, D
		doBit(m_D, 5);
		break;
	case 0x6B: // BIT 5, E
		doBit(m_E, 5);
		break;
	case 0x6C: // BIT 5, H
		doBit(m_H, 5);
		break;
	case 0x6D: // BIT 5, L
		doBit(m_L, 5);
		break;
	case 0x6E: // BIT 5, [HL]
		doBit(HL(), 5);
		break;
	case 0x6F: // BIT 5, A
		doBit(m_A, 5);
		break;
	case 0x70: // BIT 6, B
		doBit(m_B, 6);
		break;
	case 0x71: // BIT 6, C
		doBit(m_C, 6);
		break;
	case 0x72: // BIT 6, D
		doBit(m_D, 6);
		break;
	case 0x73: // BIT 6, E
		doBit(m_E, 6);
		break;
	case 0x74: // BIT 6, H
		doBit(m_H, 6);
		break;
	case 0x75: // BIT 6, L
		doBit(m_L, 6);
		break;
	case 0x76: // BIT 6, [HL]
		doBit(HL(), 6);
		break;
	case 0x77: // BIT 6, A
		doBit(m_A, 6);
		break;
	case 0x78: // BIT 7, B
		doBit(m_B, 7);
		break;
	case 0x79: // BIT 7, C
		doBit(m_C, 7);
		break;
	case 0x7A: // BIT 7, D
		doBit(m_D, 7);
		break;
	case 0x7B: // BIT 7, E
		doBit(m_E, 7);
		break;
	case 0x7C: // BIT 7, H
		doBit(m_H, 7);
		break;
	case 0x7D: // BIT 7, L
		doBit(m_L, 7);
		break;
	case 0x7E: // BIT 7, [HL]
		doBit(HL(), 7);
		break;
	case 0x7F: // BIT 7, A
		doBit(m_A, 7);
		break;
	case 0x80: // RES 0, B
		doRes(m_B, 0);
		break;
	case 0x81: // RES 0, C
		doRes(m_C, 0);
		break;
	case 0x82: // RES 0, D
		doRes(m_D, 0);
		break;
	case 0x83: // RES 0, E
		doRes(m_E, 0);
		break;
	case 0x84: // RES 0, H
		doRes(m_H, 0);
		break;
	case 0x85: // RES 0, L
		doRes(m_L, 0);
		break;
	case 0x86: // RES 0, [HL]
		doRes(HL(), 0);
		break;
	case 0x87: // RES 0, A
		doRes(m_A, 0);
		break;
	case 0x88: // RES 1, B
		doRes(m_B, 1);
		break;
	case 0x89: // RES 1, C
		doRes(m_C, 1);
		break;
	case 0x8A: // RES 1, D
		doRes(m_D, 1);
		break;
	case 0x8B: // RES 1, E
		doRes(m_E, 1);
		break;
	case 0x8C: // RES 1, H
		doRes(m_H, 1);
		break;
	case 0x8D: // RES 1, L
		doRes(m_L, 1);
		break;
	case 0x8E: // RES 1, [HL]
		doRes(HL(), 1);
		break;
	case 0x8F: // RES 1, A
		doRes(m_A, 1);
		break;
	case 0x90: // RES 2, B
		doRes(m_B, 2);
		break;
	case 0x91: // RES 2, C
		doRes(m_C, 2);
		break;
	case 0x92: // RES 2, D
		doRes(m_D, 2);
		break;
	case 0x93: // RES 2, E
		doRes(m_E, 2);
		break;
	case 0x94: // RES 2, H
		doRes(m_H, 2);
		break;
	case 0x95: // RES 2, L
		doRes(m_L, 2);
		break;
	case 0x96: // RES 2, [HL]
		doRes(HL(), 2);
		break;
	case 0x97: // RES 2, A
		doRes(m_A, 2);
		break;
	case 0x98: // RES 3, B
		doRes(m_B, 3);
		break;
	case 0x99: // RES 3, C
		doRes(m_C, 3);
		break;
	case 0x9A: // RES 3, D
		doRes(m_D, 3);
		break;
	case 0x9B: // RES 3, E
		doRes(m_E, 3);
		break;
	case 0x9C: // RES 3, H
		doRes(m_H, 3);
		break;
	case 0x9D: // RES 3, L
		doRes(m_L, 3);
		break;
	case 0x9E: // RES 3, [HL]
		doRes(HL(), 3);
		break;
	case 0x9F: // RES 3, A
		doRes(m_A, 3);
		break;
	case 0xA0: // RES 4, B
		doRes(m_B, 4);
		break;
	case 0xA1: // RES 4, C
		doRes(m_C, 4);
		break;
	case 0xA2: // RES 4, D
		doRes(m_D, 4);
		break;
	case 0xA3: // RES 4, E
		doRes(m_E, 4);
		break;
	case 0xA4: // RES 4, H
		doRes(m_H, 4);
		break;
	case 0xA5: // RES 4, L
		doRes(m_L, 4);
		break;
	case 0xA6: // RES 4, [HL]
		doRes(HL(), 4);
		break;
	case 0xA7: // RES 4, A
		doRes(m_A, 4);
		break;
	case 0xA8: // RES 5, B
		doRes(m_B, 5);
		break;
	case 0xA9: // RES 5, C
		doRes(m_C, 5);
		break;
	case 0xAA: // RES 5, D
		doRes(m_D, 5);
		break;
	case 0xAB: // RES 5, E
		doRes(m_E, 5);
		break;
	case 0xAC: // RES 5, H
		doRes(m_H, 5);
		break;
	case 0xAD: // RES 5, L
		doRes(m_L, 5);
		break;
	case 0xAE: // RES 5, [HL]
		doRes(HL(), 5);
		break;
	case 0xAF: // RES 5, A
		doRes(m_A, 5);
		break;
	case 0xB0: // RES 6, B
		doRes(m_B, 6);
		break;
	case 0xB1: // RES 6, C
		doRes(m_C, 6);
		break;
	case 0xB2: // RES 6, D
		doRes(m_D, 6);
		break;
	case 0xB3: // RES 6, E
		doRes(m_E, 6);
		break;
	case 0xB4: // RES 6, H
		doRes(m_H, 6);
		break;
	case 0xB5: // RES 6, L
		doRes(m_L, 6);
		break;
	case 0xB6: // RES 6, [HL]
		doRes(HL(), 6);
		break;
	case 0xB7: // RES 6, A
		doRes(m_A, 6);
		break;
	case 0xB8: // RES 7, B
		doRes(m_B, 7);
		break;
	case 0xB9: // RES 7, C
		doRes(m_C, 7);
		break;
	case 0xBA: // RES 7, D
		doRes(m_D, 7);
		break;
	case 0xBB: // RES 7, E
		doRes(m_E, 7);
		break;
	case 0xBC: // RES 7, H
		doRes(m_H, 7);
		break;
	case 0xBD: // RES 7, L
		doRes(m_L, 7);
		break;
	case 0xBE: // RES 7, [HL]
		doRes(HL(), 7);
		break;
	case 0xBF: // RES 7, A
		doRes(m_A, 7);
		break;
	case 0xC0: // SET 0, B
		doSet(m_B, 0);
		break;
	case 0xC1: // SET 0, C
		doSet(m_C, 0);
		break;
	case 0xC2: // SET 0, D
		doSet(m_D, 0);
		break;
	case 0xC3: // SET 0, E
		doSet(m_E, 0);
		break;
	case 0xC4: // SET 0, H
		doSet(m_H, 0);
		break;
	case 0xC5: // SET 0, L
		doSet(m_L, 0);
		break;
	case 0xC6: // SET 0, [HL]
		doSet(HL(), 0);
		break;
	case 0xC7: // SET 0, A
		doSet(m_A, 0);
		break;
	case 0xC8: // SET 1, B
		doSet(m_B, 1);
		break;
	case 0xC9: // SET 1, C
		doSet(m_C, 1);
		break;
	case 0xCA: // SET 1, D
		doSet(m_D, 1);
		break;
	case 0xCB: // SET 1, E
		doSet(m_E, 1);
		break;
	case 0xCC: // SET 1, H
		doSet(m_H, 1);
		break;
	case 0xCD: // SET 1, L
		doSet(m_L, 1);
		break;
	case 0xCE: // SET 1, [HL]
		doSet(HL(), 1);
		break;
	case 0xCF: // SET 1, A
		doSet(m_A, 1);
		break;
	case 0xD0: // SET 2, B
		doSet(m_B, 2);
		break;
	case 0xD1: // SET 2, C
		doSet(m_C, 2);
		break;
	case 0xD2: // SET 2, D
		doSet(m_D, 2);
		break;
	case 0xD3: // SET 2, E
		doSet(m_E, 2);
		break;
	case 0xD4: // SET 2, H
		doSet(m_H, 2);
		break;
	case 0xD5: // SET 2, L
		doSet(m_L, 2);
		break;
	case 0xD6: // SET 2, [HL]
		doSet(HL(), 2);
		break;
	case 0xD7: // SET 2, A
		doSet(m_A, 2);
		break;
	case 0xD8: // SET 3, B
		doSet(m_B, 3);
		break;
	case 0xD9: // SET 3, C
		doSet(m_C, 3);
		break;
	case 0xDA: // SET 3, D
		doSet(m_D, 3);
		break;
	case 0xDB: // SET 3, E
		doSet(m_E, 3);
		break;
	case 0xDC: // SET 3, H
		doSet(m_H, 3);
		break;
	case 0xDD: // SET 3, L
		doSet(m_L, 3);
		break;
	case 0xDE: // SET 3, [HL]
		doSet(HL(), 3);
		break;
	case 0xDF: // SET 3, A
		doSet(m_A, 3);
		break;
	case 0xE0: // SET 4, B
		doSet(m_B, 4);
		break;
	case 0xE1: // SET 4, C
		doSet(m_C, 4);
		break;
	case 0xE2: // SET 4, D
		doSet(m_D, 4);
		break;
	case 0xE3: // SET 4, E
		doSet(m_E, 4);
		break;
	case 0xE4: // SET 4, H
		doSet(m_H, 4);
		break;
	case 0xE5: // SET 4, L
		doSet(m_L, 4);
		break;
	case 0xE6: // SET 4, [HL]
		doSet(HL(), 4);
		break;
	case 0xE7: // SET 4, A
		doSet(m_A, 4);
		break;
	case 0xE8: // SET 5, B
		doSet(m_B, 5);
		break;
	case 0xE9: // SET 5, C
		doSet(m_C, 5);
		break;
	case 0xEA: // SET 5, D
		doSet(m_D, 5);
		break;
	case 0xEB: // SET 5, E
		doSet(m_E, 5);
		break;
	case 0xEC: // SET 5, H
		doSet(m_H, 5);
		break;
	case 0xED: // SET 5, L
		doSet(m_L, 5);
		break;
	case 0xEE: // SET 5, [HL]
		doSet(HL(), 5);
		break;
	case 0xEF: // SET 5, A
		doSet(m_A, 5);
		break;
	case 0xF0: // SET 6, B
		doSet(m_B, 6);
		break;
	case 0xF1: // SET 6, C
		doSet(m_C, 6);
		break;
	case 0xF2: // SET 6, D
		doSet(m_D, 6);
		break;
	case 0xF3: // SET 6, E
		doSet(m_E, 6);
		break;
	case 0xF4: // SET 6, H
		doSet(m_H, 6);
		break;
	case 0xF5: // SET 6, L
		doSet(m_L, 6);
		break;
	case 0xF6: // SET 6, [HL]
		doSet(HL(), 6);
		break;
	case 0xF7: // SET 6, A
		doSet(m_A, 6);
		break;
	case 0xF8: // SET 7, B
		doSet(m_B, 7);
		break;
	case 0xF9: // SET 7, C
		doSet(m_C, 7);
		break;
	case 0xFA: // SET 7, D
		doSet(m_D, 7);
		break;
	case 0xFB: // SET 7, E
		doSet(m_E, 7);
		break;
	case 0xFC: // SET 7, H
		doSet(m_H, 7);
		break;
	case 0xFD: // SET 7, L
		doSet(m_L, 7);
		break;
	case 0xFE: // SET 7, [HL]
		doSet(HL(), 7);
		break;
	case 0xFF: // SET 7, A
		doSet(m_A, 7);
		break;
	default: {
		std::stringstream ss;
		ss << "Unhandled prefixed instruction 0x" << std::hex << std::setw(2) << std::setfill('0') << int(opcode)
		   << " at PC=0x" << std::hex << std::setw(4) << std::setfill('0') << int(m_PC) << ": \""
		   << CPU_CB_INSTRUCTION_MNEMONICS.at(opcode) << "\"";

		throw std::runtime_error(ss.str());
	}
	}

	return CPU_CB_INSTRUCTION_CYCLES.at(opcode);
}
