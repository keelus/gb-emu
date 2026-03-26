#pragma once

#include "cartridge.hpp"
#include "memory.hpp"
#include "timer.hpp"
#include "joypad.hpp"
#include <cstdint>
#include <cstring>

#define IO_MEM_SIZE 128

class Cpu;
class Ppu;

class Bus {
  public:
	enum class InterruptRequestType {
		Joypad,
		Serial,
		Timer,
		Lcd,
		VBlank,
	};
	Bus() {
		m_cartridge = NULL;
		m_cpu = NULL;
		m_joypad = NULL;
		m_memory = NULL;
		m_ppu = NULL;
		m_timer = NULL;

		m_oamSourceAndStart = 0;
	}

	void addCartridge(Cartridge *cartridge) { m_cartridge = cartridge; }
	void addCpu(Cpu *cpu) { m_cpu = cpu; }
	void addJoypad(Joypad *joypad) { m_joypad = joypad; }
	void addMemory(Memory *memory) { m_memory = memory; }
	void addPpu(Ppu *ppu) { m_ppu = ppu; }
	void addTimer(Timer *timer) { m_timer = timer; }

	void doDmaTransfer(void);

	uint8_t read8(const uint16_t address) const;
	uint16_t read16(const uint16_t address) const;

	void write8(const uint16_t address, const uint8_t value);
	void write16(const uint16_t address, const uint16_t value);

	void requestInterrupt(InterruptRequestType interruptType);

  private:
	uint8_t ioRead8(const uint16_t address) const;
	void ioWrite8(const uint16_t address, const uint8_t value);

	Cartridge *m_cartridge;
	Cpu *m_cpu;
	Joypad *m_joypad;
	Memory *m_memory;
	Ppu *m_ppu;
	Timer *m_timer;

	uint8_t m_oamSourceAndStart;
};
