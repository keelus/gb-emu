#pragma once

#include <chrono>
#include <gtkmm.h>
#include <gtkmm/window.h>
#include <ratio>
#include <thread>

#include "config.hpp"
#include "gameboy.hpp"
#include "graphics/background_fifo.hpp"
#include "joypad.hpp"
#include "menu_bar/menu_bar.hpp"
#include "platform.hpp"
#include "subsystems/opengl.hpp"
#include "subsystems/portaudio.hpp"

class PlatformGtk : public Platform, public Gtk::Window {
  public:
	PlatformGtk(int argc, char *argv[]) : m_menuBar(*this) { setupWindow(); }

	~PlatformGtk() {}

	void addGameBoy(GameBoy *gameBoy, const std::string &romName);
	void resetGameBoy();
	void removeGameBoy();

	void stop() { m_running = false; }
	bool running() const override { return m_running; }

	std::chrono::time_point<std::chrono::steady_clock> m_frameStart;
	void beforeFrame() override { m_frameStart = std::chrono::steady_clock::now(); }
	void afterFrame() override {
		using namespace std::chrono;

		auto frameMs = std::chrono::steady_clock::now() - m_frameStart;
		if(frameMs < duration<float, std::milli>(GameBoy::MS_PER_FRAME)) {
			std::this_thread::sleep_for(duration<float, std::milli>(GameBoy::MS_PER_FRAME) - frameMs);
		}
	}

	void drawPixel(uint8_t x, uint8_t y, Color color) override { m_openGlSubsystem.drawPixel(x, y, color); }

	void showFrame() override { m_openGlSubsystem.queueRender(); }

	void swapBuffers() override {
		m_openGlSubsystem.swapBuffers();
		activeColorPalette = m_nextActiveColorPalette;
	}

	float getAudioAmplitude() const override { return 1.0; }
	float getAudioSampleRate() const override { return m_portAudioSubsystem.audioSampleRate(); }
	void pushAudioSample(float sample) override { m_portAudioSubsystem.pushSample(sample); }
	void muteAudio() override { m_portAudioSubsystem.pause(); }
	void unmuteAudio() override { m_portAudioSubsystem.unPause(); }
	void resetAudio() { m_portAudioSubsystem.reset(); }

	void handleKeyPressed(guint keyVal) { handleKey(keyVal, true); }
	void handleKeyReleased(guint keyVal) { handleKey(keyVal, false); }

  private:
	void setupWindow();
	void setupKeyController();

	bool tick() {
		if(!m_gameBoy) { return true; }

		while(Config::skipIntro && !m_gameBoy->introEnded()) {
			m_gameBoy->tick();
		}

		int cycles = 0;
		while(cycles < GameBoy::CYCLES_PER_FRAME) {
			cycles += m_gameBoy->tick();
		}

		showFrame();
		return true;
	}

	void handleKey(guint keyVal, bool pressed) {
		if(!m_gameBoy) { return; }

		Joypad::Key key;
		switch(keyVal) {
		case GDK_KEY_0: m_nextActiveColorPalette = 0; return;
		case GDK_KEY_1: m_nextActiveColorPalette = 1; return;
		case GDK_KEY_2: m_nextActiveColorPalette = 2; return;
		case GDK_KEY_Up: key = Joypad::Key::Up; break;
		case GDK_KEY_Down: key = Joypad::Key::Down; break;
		case GDK_KEY_Left: key = Joypad::Key::Left; break;
		case GDK_KEY_Right: key = Joypad::Key::Right; break;
		case GDK_KEY_Return: key = Joypad::Key::Start; break;
		case GDK_KEY_BackSpace: key = Joypad::Key::Select; break;
		case GDK_KEY_a: key = Joypad::Key::A; break;
		case GDK_KEY_b: key = Joypad::Key::B; break;
		default: return;
		}

		if(pressed) {
			m_gameBoy->handleKeydown(key);
		} else {
			m_gameBoy->handleKeyup(key);
		}
	}

	uint8_t m_nextActiveColorPalette = activeColorPalette;

	bool m_running = true;

	Gtk::Label m_noRomLabel{"There is no ROM loaded"};

	GameBoy *m_gameBoy = nullptr;

	GtkMenuBar::MenuBar m_menuBar;

	OpenGlSubsystem m_openGlSubsystem;
	PortAudioSubsystem m_portAudioSubsystem;
};
