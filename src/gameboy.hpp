#pragma once

#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "memory.hpp"

class GameBoy {
  public:
	GameBoy(const std::string &romPath) : m_cartridge(romPath), m_cpu(m_bus) {
		m_bus.addCpu(&m_cpu);
		m_bus.addMemory(&m_memory);
		m_bus.addCartridge(&m_cartridge);
	}

	void start(void);

  private:
	Bus m_bus;
	Cartridge m_cartridge;
	Cpu m_cpu;
	Memory m_memory;
};
