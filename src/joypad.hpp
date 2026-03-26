#pragma once

#include "SDL_keycode.h"
#include <bitset>
#include <cstdint>

class Joypad {
  public:
	Joypad() {
		m_selectedOutput = SelectedOutput::None;
		m_buttons = 0xF;
		m_dPad = 0xF;
	}

	void write8(const uint8_t value) {
		uint8_t outputBits = value & 0x30;
		if(outputBits == 0x30) {
			m_selectedOutput = SelectedOutput::None;
		} else if(outputBits == 0x20) {
			m_selectedOutput = SelectedOutput::DPad;
		} else {
			m_selectedOutput = SelectedOutput::Buttons;
		}
	}

	uint8_t read8() const {
		switch(m_selectedOutput) {
		case SelectedOutput::Buttons: return 0x30 | (static_cast<uint8_t>(m_buttons.to_ulong()) & 0xF); break;
		case SelectedOutput::DPad: return 0x30 | (static_cast<uint8_t>(m_dPad.to_ulong()) & 0xF); break;
		default: return 0x3F;
		}
	}

	void handleKeyDown(SDL_Keycode keyCode) { handleKey(keyCode, false); }
	void handleKeyUp(SDL_Keycode keyCode) { handleKey(keyCode, true); }

  private:
	enum SelectedOutput { None = 0, Buttons, DPad };

	void handleKey(SDL_Keycode keyCode, bool isKeyUp) {
		switch(keyCode) {
		case SDLK_DOWN: m_dPad.set(3, isKeyUp); break;
		case SDLK_UP: m_dPad.set(2, isKeyUp); break;
		case SDLK_LEFT: m_dPad.set(1, isKeyUp); break;
		case SDLK_RIGHT: m_dPad.set(0, isKeyUp); break;

		case SDLK_RETURN: m_buttons.set(3, isKeyUp); break;
		case SDLK_BACKSPACE: m_buttons.set(2, isKeyUp); break;
		case SDLK_b: m_buttons.set(1, isKeyUp); break;
		case SDLK_a: m_buttons.set(0, isKeyUp); break;
		default: break;
		}
	}

	std::bitset<4> m_buttons, m_dPad;
	SelectedOutput m_selectedOutput;
};
