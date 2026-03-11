#pragma once

#include "cartridge.hpp"
#include "memory.hpp"
#include <cstdint>

class Cpu;

class Bus {
  public:
	Bus() {
		m_memory = NULL;
		m_cpu = NULL;
		m_cartridge = NULL;
	}

	void addCpu(Cpu *cpu) { m_cpu = cpu; }
	void addMemory(Memory *memory) { m_memory = memory; }
	void addCartridge(Cartridge *cartridge) { m_cartridge = cartridge; }

	uint8_t read8(const uint16_t address) const;
	uint16_t read16(const uint16_t address) const;

	void write8(const uint16_t address, const uint8_t value);
	void write16(const uint16_t address, const uint16_t value);

  private:
	Memory *m_memory;
	Cpu *m_cpu;
	Cartridge *m_cartridge;
};
