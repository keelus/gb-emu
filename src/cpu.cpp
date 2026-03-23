#include "cpu.hpp"

void Cpu::handleInterrupts(void) {
	if(m_IME == 0) {
		if((m_interruptEnable & m_interruptFlag) != 0) {
			m_halted = false;
			return;
		}
		return;
	}
	if((m_interruptEnable & m_interruptFlag & 0x1F) != 0) { m_halted = false; }


	if(getInterruptEnable<InterruptFlag::VBlank>() && getInterruptFlag<InterruptFlag::VBlank>()) {
		doPush(m_PC);
		m_PC = 0x40;
		setInterruptFlag<InterruptFlag::VBlank>(0);
		m_IME = 0;
	}

	if(getInterruptEnable<InterruptFlag::Lcd>() && getInterruptFlag<InterruptFlag::Lcd>()) {
		doPush(m_PC);
		m_PC = 0x48;
		setInterruptFlag<InterruptFlag::Lcd>(0);
		m_IME = 0;
	}

	if(getInterruptEnable<InterruptFlag::Timer>() && getInterruptFlag<InterruptFlag::Timer>()) {
		doPush(m_PC);
		m_PC = 0x50;
		setInterruptFlag<InterruptFlag::Timer>(0);
		m_IME = 0;
	}

	if(getInterruptEnable<InterruptFlag::Serial>() && getInterruptFlag<InterruptFlag::Serial>()) {
		doPush(m_PC);
		m_PC = 0x58;
		setInterruptFlag<InterruptFlag::Serial>(0);
		m_IME = 0;
	}

	if(getInterruptEnable<InterruptFlag::Joypad>() && getInterruptFlag<InterruptFlag::Joypad>()) {
		doPush(m_PC);
		m_PC = 0x60;
		setInterruptFlag<InterruptFlag::Joypad>(0);
		m_IME = 0;
	}
}
