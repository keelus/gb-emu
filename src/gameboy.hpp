#pragma once

#include "SDL_keycode.h"
#include "audio/apu.hpp"
#include "bus.hpp"
#include "cartridge/cartridge.hpp"
#include "cpu/cpu.hpp"
#include "graphics/lcd.hpp"
#include "joypad.hpp"
#include "memory.hpp"
#include "graphics/ppu.hpp"
#include "timer.hpp"
#include <cstdint>
#include <memory>

class GameBoy {
  public:
	GameBoy(const std::string &cartridgePath) : m_cpu(m_bus), m_ppu(m_bus, m_lcd), m_timer(m_bus) {
		m_cartridge = Cartridge::createCartridge(cartridgePath);

		m_bus.addApu(&m_apu);
		m_bus.addCartridge(m_cartridge.get());
		m_bus.addCpu(&m_cpu);
		m_bus.addJoypad(&m_joypad);
		m_bus.addMemory(&m_memory);
		m_bus.addPpu(&m_ppu);
		m_bus.addTimer(&m_timer);
	}

	void start(void);
	void debugCartridge(void) const { m_cartridge->debug(); }

	int tick();
	void dump(void) { m_cpu.dump(); }

	void handleKeydown(SDL_Keycode keyCode) { m_joypad.handleKeyDown(keyCode); }
	void handleKeyup(SDL_Keycode keyCode) { m_joypad.handleKeyUp(keyCode); }

	bool introEnded() const { return m_bus.introEnded(); }

	const uint32_t *getLcdBuffer() const { return m_lcd.getBuffer(); }

  private:
	Apu m_apu;
	Bus m_bus;
	Cpu m_cpu;
	Joypad m_joypad;
	Lcd m_lcd;
	Memory m_memory;
	Ppu m_ppu;
	Timer m_timer;
	std::unique_ptr<Cartridge> m_cartridge;
};
