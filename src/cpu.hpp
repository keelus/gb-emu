#pragma once

#include <cstdint>

#include "alu.hpp"
#include "bus.hpp"

class Cpu {
  public:
	Cpu(Bus &bus) : m_bus(bus) {
		m_A = 0;
		m_F = 0;

		m_B = 0;
		m_C = 0;

		m_D = 0;
		m_E = 0;

		m_H = 0;
		m_L = 0;

		m_SP = 0xFFFE;
		// m_PC = 0x100;
		m_PC = 0;

		m_IME = false;

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
	int executeCbInstruction(void);

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

	void doSub(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::sub8(a, b);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(1);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}
	void doSbc(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::sub8WithCarry(a, b, getFlag<Flag::C>());
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(1);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}

	void doAnd(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::gateAnd(a, b);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(1);
		setFlag<Flag::C>(0);
	}
	void doXor(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::gateXor(a, b);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(0);
	}
	void doOr(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::gateOr(a, b);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(0);
	}
	void doCp(uint8_t &a, const uint8_t b) {
		ALU::Result8 res = ALU::sub8(a, b);
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(1);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}

	void doInc(uint8_t &a) {
		ALU::Result8 res = ALU::inc8(a);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}
	void doInc(uint16_t address) {
		uint8_t value = m_bus.read8(address);

		ALU::Result8 res = ALU::inc8(value);
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);

		m_bus.write8(address, res.value);
	}

	void doInc16(uint16_t &value) { value = ALU::inc16(value).value; }
	void doInc16(uint8_t &upper, uint8_t &lower) {
		uint16_t value = (static_cast<uint16_t>(upper) << 8) | static_cast<uint16_t>(lower);
		uint16_t result = ALU::inc16(value).value;

		upper = static_cast<uint8_t>(result >> 8);
		lower = static_cast<uint8_t>(result);
	}

	void doDec(uint8_t &a) {
		ALU::Result8 res = ALU::dec8(a);
		a = res.value;
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(1);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);
	}
	void doDec(uint16_t address) {
		uint16_t value = m_bus.read8(address);

		ALU::Result8 res = ALU::dec8(value);
		setFlag<Flag::Z>(res.flag_z);
		setFlag<Flag::N>(1);
		setFlag<Flag::H>(res.flag_h);
		setFlag<Flag::C>(res.flag_c);

		m_bus.write8(address, value);
	}
	void doDec(uint8_t &upper, uint8_t lower) {
		uint16_t value = (static_cast<uint16_t>(upper) << 8) | static_cast<uint16_t>(lower);
		uint16_t result = ALU::dec16(value).value;

		upper = static_cast<uint8_t>(result >> 8);
		lower = static_cast<uint8_t>(result);
	}

	void doDec16(uint16_t &value) { value = ALU::dec16(value).value; }
	void doDec16(uint8_t &upper, uint8_t &lower) {
		uint16_t value = (static_cast<uint16_t>(upper) << 8) | static_cast<uint16_t>(lower);
		uint16_t result = ALU::dec16(value).value;

		upper = static_cast<uint8_t>(result >> 8);
		lower = static_cast<uint8_t>(result);
	}

	void doJr() {
		int8_t offset = m_bus.read8(m_PC++);
		m_PC = static_cast<uint16_t>(static_cast<int32_t>(m_PC) + offset);
	}
	bool doJr(bool condition) {
		int8_t offset = m_bus.read8(m_PC++);
		if(condition) { m_PC = static_cast<uint16_t>(static_cast<int32_t>(m_PC) + offset); }
		return condition;
	}

	void doJp() { m_PC = m_bus.read16(m_PC); }
	bool doJp(bool condition) {
		uint16_t address = m_bus.read16(m_PC);
		m_PC += 2;
		if(condition) { m_PC = address; }
		return condition;
	}

	void doPush(uint16_t value) {
		m_bus.write8(--m_SP, static_cast<uint8_t>(value >> 8));
		m_bus.write8(--m_SP, static_cast<uint8_t>(value));
	}
	void doPop(uint16_t &value) {
		uint8_t upper, lower;
		doPop(upper, lower);
		value = (static_cast<uint16_t>(upper) << 8) | static_cast<uint16_t>(lower);
	}
	void doPop(uint8_t &upper, uint8_t &lower) {
		lower = m_bus.read8(m_SP++);
		upper = m_bus.read8(m_SP++);
	}

	void doCall() { doCall(true); }
	bool doCall(bool condition) {
		uint16_t address = m_bus.read16(m_PC);
		m_PC += 2;

		if(condition) {
			doPush(m_PC);
			m_PC = address;
		}
		return condition;
	}

	void doRet() { doPop(m_PC); }
	bool doRet(bool condition) {
		if(condition) { doPop(m_PC); }
		return condition;
	}

	void doRlc(uint8_t &reg) {
		ALU::Result8 res = ALU::rlc(reg);
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doRlc(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doRlc(value);
		m_bus.write8(address, value);
	}

	void doRlca() {
		doRlc(m_A);
		setFlag<Flag::Z>(0);
	}

	void doRrc(uint8_t &reg) {
		ALU::Result8 res = ALU::rrc(reg);
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doRrc(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doRrc(value);
		m_bus.write8(address, value);
	}

	void doRrca() {
		doRrc(m_A);
		setFlag<Flag::Z>(0);
	}

	void doRl(uint8_t &reg) {
		ALU::Result8 res = ALU::rl(reg, getFlag<Flag::C>());
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doRl(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doRl(value);
		m_bus.write8(address, value);
	}

	void doRla() {
		doRl(m_A);
		setFlag<Flag::Z>(0);
	}

	void doRr(uint8_t &reg) {
		ALU::Result8 res = ALU::rr(reg, getFlag<Flag::C>());
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doRr(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doRr(value);
		m_bus.write8(address, value);
	}

	void doRra() {
		doRr(m_A);
		setFlag<Flag::Z>(0);
	}

	void doSla(uint8_t &reg) {
		ALU::Result8 res = ALU::sla(reg);
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doSla(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doSla(value);
		m_bus.write8(address, value);
	}

	void doSra(uint8_t &reg) {
		ALU::Result8 res = ALU::sra(reg);
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doSra(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doSra(value);
		m_bus.write8(address, value);
	}

	void doSwap(uint8_t &reg) {
		reg = ((reg & 0xF) << 4) | ((reg & 0xF0) >> 4);
		setFlag<Flag::Z>(reg == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(0);
	}

	void doSwap(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doSwap(value);
		m_bus.write8(address, value);
	}

	void doSrl(uint8_t &reg) {
		ALU::Result8 res = ALU::srl(reg);
		reg = res.value;
		setFlag<Flag::Z>(res.value == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(0);
		setFlag<Flag::C>(res.flag_c);
	}

	void doSrl(uint16_t address) {
		uint8_t value = m_bus.read8(address);
		doSrl(value);
		m_bus.write8(address, value);
	}

	void doBit(uint8_t &reg, uint8_t bit) {
		setFlag<Flag::Z>(((reg >> bit) & 1) == 0);
		setFlag<Flag::N>(0);
		setFlag<Flag::H>(1);
	}

	void doBit(uint16_t address, uint8_t bit) {
		uint8_t value = m_bus.read8(address);
		doBit(value, bit);
		m_bus.write8(address, value);
	}

	void doRes(uint8_t &reg, uint8_t bit) { reg &= ~(1 << bit); }

	void doRes(uint16_t address, uint8_t bit) {
		uint8_t value = m_bus.read8(address);
		doRes(value, bit);
		m_bus.write8(address, value);
	}

	void doSet(uint8_t &reg, uint8_t bit) { reg |= (1 << bit); }

	void doSet(uint16_t address, uint8_t bit) {
		uint8_t value = m_bus.read8(address);
		doSet(value, bit);
		m_bus.write8(address, value);
	}

	uint8_t m_A, m_F;
	uint8_t m_B, m_C;
	uint8_t m_D, m_E;
	uint8_t m_H, m_L;
	uint16_t m_SP, m_PC;

	bool m_IME;

	bool m_halted;

	Bus &m_bus;
};
