#pragma once

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
#include <fstream>
#include <memory>

class GameBoy {
  public:
	GameBoy(const std::string &cartridgePath, Platform &platform)
		: m_apu(platform), m_cpu(m_bus), m_ppu(m_bus, m_lcd), m_lcd(platform), m_timer(m_bus) {
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

	void handleKeydown(Joypad::Key key) { m_joypad.handleKeyDown(key); }
	void handleKeyup(Joypad::Key key) { m_joypad.handleKeyUp(key); }

	bool introEnded() const { return m_bus.introEnded(); }

	void reset() {
		m_apu.reset();
		m_bus.reset();
		m_cpu.reset();
		m_joypad.reset();
		m_lcd.resetScreenX();
		m_memory.reset();
		m_ppu.reset();
		m_timer.reset();
		m_cartridge->reset();

		m_prevDiv = 0;
	}

	void loadCustomBootRom(const std::string &bootRomPath) {
		std::ifstream bootRomFile(bootRomPath, std::ios::binary | std::ios::ate);
		if(!bootRomFile) { throw std::runtime_error("Could not open the custom boot ROM file."); }

		std::streamsize bootRomSize = bootRomFile.tellg();
		if(bootRomSize != 256) { throw std::runtime_error("The boot ROM's size must be exactly 256 bytes long."); }

		std::array<char, 256> bootRom;
		bootRomFile.seekg(0, std::ios::beg);
		if(!bootRomFile.read(bootRom.data(), 256)) {
			throw std::runtime_error("Failed to read from the custom boot ROM file.");
		}

		m_cartridge->setCustomBootRom(bootRom.data());
	}

	void disableCustomBootRom() { m_cartridge->disableCustomBootRom(); }

	static constexpr float FRAMES_PER_SECOND = 59.7f;
	static constexpr float CYCLES_PER_FRAME = (Cpu::CLOCK_SPEED / FRAMES_PER_SECOND);
	static constexpr float MS_PER_FRAME = 1 / FRAMES_PER_SECOND * 1000;

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

	uint16_t m_prevDiv = 0;
};
