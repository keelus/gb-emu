#pragma once

#include "catridge.hpp"
#include "cpu.hpp"
#include "memory.hpp"

class GameBoy {
  public:
	GameBoy(const std::string &romPath) : m_catridge(romPath), m_memory(m_catridge), m_cpu(m_memory) {}
	void start(void);

  private:
	Catridge m_catridge;
	Memory m_memory;
	Cpu m_cpu;
};
