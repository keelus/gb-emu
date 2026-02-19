#pragma once

#include <cstdint>

#include "alu.hpp"
#include "memory.hpp"

class Cpu {
  public:
	Cpu(Memory &memory) : m_memory(memory) {
		m_A = 0;
		m_F = 0;

		m_B = 0;
		m_C = 0;

		m_D = 0;
		m_E = 0;

		m_H = 0;
		m_L = 0;

		m_SP = 0;
		m_PC = 0x100;

		m_halted = false;
	}

	enum class Flag : uint8_t {
		Z = 0x80,
		N = 0x40,
		H = 0x20,
		C = 0x10,
	};

	uint8_t A(void) const { return m_A; }
	uint8_t F(void) const { return m_F; }
	uint16_t AF(void) const { return (static_cast<uint16_t>(m_A) << 8) | static_cast<uint16_t>(m_F); }

	template <Flag Fbit> bool getFlag(void) const { return (m_F & static_cast<uint8_t>(Fbit)) != 0; }
	template <Flag Fbit> void setFlag(bool value) {
		if(value) {
			m_F |= static_cast<uint8_t>(Fbit);
		} else {
			m_F &= ~(static_cast<uint8_t>(Fbit));
		}
	}

	uint8_t B(void) const { return m_B; }
	uint8_t C(void) const { return m_C; }
	uint16_t BC(void) const { return (static_cast<uint16_t>(m_B) << 8) | static_cast<uint16_t>(m_C); }

	uint8_t D(void) const { return m_D; }
	uint8_t E(void) const { return m_E; }
	uint16_t DE(void) const { return (static_cast<uint16_t>(m_D) << 8) | static_cast<uint16_t>(m_E); }

	uint8_t H(void) const { return m_H; }
	uint8_t L(void) const { return m_L; }
	uint16_t HL(void) const { return (static_cast<uint16_t>(m_H) << 8) | static_cast<uint16_t>(m_L); }

	uint16_t SP(void) const { return m_SP; }
	uint16_t PC(void) const { return m_PC; }

	int executeInstruction(void);

  private:
	void incHL(void) {
		uint16_t hl = HL() + 1;
		m_H = static_cast<uint8_t>(hl >> 8);
		m_L = static_cast<uint8_t>(hl);
	}
	void decHL(void) {
		uint16_t hl = HL() - 1;
		m_H = static_cast<uint8_t>(hl >> 8);
		m_L = static_cast<uint8_t>(hl);
	}

	void doLd(uint8_t &a, const uint8_t b) { a = b; }
	void doAdd(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::add8(a, b);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}
	void doAdd16ToHL(const uint16_t b) {
		ALU::Result16 res = ALU::add16(HL(), b);
		m_H = static_cast<uint8_t>(res.value >> 8);
		m_L = static_cast<uint8_t>(res.value);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}
	void doAdc(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::add8WithCarry(a, b, getFlag<Flag::C>());
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}

	uint8_t m_A, m_F;
	uint8_t m_B, m_C;
	uint8_t m_D, m_E;
	uint8_t m_H, m_L;
	uint16_t m_SP, m_PC;

	bool m_halted;

	Memory &m_memory;
};
