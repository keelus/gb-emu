#pragma once

#include "SDL_keycode.h"
#include "apu.hpp"
#include "bus.hpp"
#include "cartridge.hpp"
#include "cpu.hpp"
#include "joypad.hpp"
#include "memory.hpp"
#include "ppu.hpp"
#include "timer.hpp"

class GameBoy {
  public:
	GameBoy(const std::string &romPath) : m_cartridge(romPath), m_cpu(m_bus), m_ppu(m_bus), m_timer(m_bus) {
		m_bus.addCartridge(&m_cartridge);
		m_bus.addCpu(&m_cpu);
		m_bus.addJoypad(&m_joypad);
		m_bus.addMemory(&m_memory);
		m_bus.addPpu(&m_ppu);
		m_bus.addTimer(&m_timer);
		m_bus.addApu(&m_apu);
	}

	void start(void);
	void debugCartridge(void) const { m_cartridge.debug(); }

	int tick();
	void dump(void) { m_cpu.dump(); }

	void handleKeydown(SDL_Keycode keyCode) { m_joypad.handleKeyDown(keyCode); }
	void handleKeyup(SDL_Keycode keyCode) { m_joypad.handleKeyUp(keyCode); }

	bool introEnded() const { return m_bus.introEnded(); }

  private:
	Apu m_apu;
	Bus m_bus;
	Cartridge m_cartridge;
	Cpu m_cpu;
	Memory m_memory;
	Ppu m_ppu;
	Timer m_timer;
	Joypad m_joypad;
};
