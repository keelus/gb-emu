#pragma once

#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "timer.hpp"

class GameBoy {
  public:
	GameBoy(const std::string &romPath) : m_cartridge(romPath), m_cpu(m_bus), m_ppu(m_bus) {
		m_bus.addCartridge(&m_cartridge);
		m_bus.addCpu(&m_cpu);
		m_bus.addMemory(&m_memory);
		m_bus.addPpu(&m_ppu);
		m_bus.addTimer(&m_timer);
	}

	void start(void);
	void debugCartridge(void) const { m_cartridge.debug(); }

	int tick();
	void dump(void) { m_cpu.dump(); }

  private:
	Bus m_bus;
	Cartridge m_cartridge;
	Cpu m_cpu;
	Memory m_memory;
	Ppu m_ppu;
	Timer m_timer;
};
