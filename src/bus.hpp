#pragma once

#include "cartridge.hpp"
#include "memory.hpp"
#include <cstdint>
#include <cstring>

#define IO_MEM_SIZE 128

class Cpu;
class Ppu;

class Bus {
  public:
	Bus() {
		m_memory = NULL;
		m_cpu = NULL;
		m_cartridge = NULL;

		memset(m_iomem, 0, sizeof(uint8_t) * IO_MEM_SIZE);
	}

	void addCartridge(Cartridge *cartridge) { m_cartridge = cartridge; }
	void addCpu(Cpu *cpu) { m_cpu = cpu; }
	void addMemory(Memory *memory) { m_memory = memory; }
	void addPpu(Ppu *ppu) { m_ppu = ppu; }

	uint8_t read8(const uint16_t address) const;
	uint16_t read16(const uint16_t address) const;

	void write8(const uint16_t address, const uint8_t value);
	void write16(const uint16_t address, const uint16_t value);

  private:
	uint8_t ioRead8(const uint16_t address) const;
	void ioWrite8(const uint16_t address, const uint8_t value);

	Cartridge *m_cartridge;
	Cpu *m_cpu;
	Memory *m_memory;
	Ppu *m_ppu;

	uint8_t m_iomem[IO_MEM_SIZE]; // Temporal
	uint8_t m_audioMem[23];		  // Temporal
};
