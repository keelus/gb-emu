#pragma once

#include <chrono>
#include <glibmm/main.h>
#include <gtkmm.h>
#include <gtkmm/window.h>
#include <memory>
#include <ratio>
#include <stdexcept>
#include <thread>
#include <zirc/config.hpp>
#include <zirc/gameboy.hpp>
#include <zirc/graphics/background_fifo.hpp>
#include <zirc/joypad.hpp>
#include <zirc/platform.hpp>

#include "backends/audio/audio_backend.hpp"
#include "backends/audio/portaudio.hpp"
#include "backends/audio/sdl2.hpp"
#include "backends/video/opengl.hpp"
#include "backends/video/software.hpp"
#include "backends/video/video_backend.hpp"
#include "gtk_config.hpp"
#include "menu_bar/menu_bar.hpp"
#include "preferences_window.hpp"

class PlatformGtk : public Zirc::Platform, public Gtk::Window {
  public:
	PlatformGtk(int argc, char *argv[]) : m_menuBar(*this) {
		setupWindow();
		reloadVideoBackend();
		m_videoBackend->setVisible(false);
		reloadAudioBackend();
	}

	~PlatformGtk() {}

	void addGameBoy(Zirc::GameBoy *gameBoy, const std::string &romName);
	void resetGameBoy();
	void removeGameBoy();

	void reloadVideoBackend() {
		if(m_videoBackend) {
			m_contentBox.remove(*m_videoBackend->getGtkWidget());
			m_videoBackend.reset();
		}

		if(GtkConfig::get().videoBackend == 0) {
			m_videoBackend = std::make_unique<VideoBackendOpenGl>();
		} else if(GtkConfig::get().videoBackend == 1) {
			m_videoBackend = std::make_unique<VideoBackendSoftware>();
		} else {
			throw std::runtime_error("Unknown video backend.");
		}

		m_videoBackend->initialize();
		m_contentBox.append(*m_videoBackend->getGtkWidget());
	}

	void reloadAudioBackend() {
		bool wasPaused = false;
		if(m_audioBackend) {
			wasPaused = m_audioBackend->isPaused();
			m_audioBackend.reset();
		}

		if(GtkConfig::get().audioBackend == 0) {
			m_audioBackend = std::make_unique<AudioBackendPortAudio>();
		} else if(GtkConfig::get().audioBackend == 1) {
			m_audioBackend = std::make_unique<AudioBackendSdl2>();
		} else {
			throw std::runtime_error("Unknown audio backend.");
		}

		m_audioBackend->initialize();
		if(!wasPaused) { m_audioBackend->unPause(); }
	}

	void stop() { m_running = false; }
	bool running() const override { return m_running; }

	std::chrono::time_point<std::chrono::steady_clock> m_frameStart;
	void beforeFrame() override { m_frameStart = std::chrono::steady_clock::now(); }
	void afterFrame() override {
		using namespace std::chrono;

		auto frameMs = std::chrono::steady_clock::now() - m_frameStart;
		if(frameMs < duration<float, std::milli>(Zirc::GameBoy::MS_PER_FRAME)) {
			std::this_thread::sleep_for(duration<float, std::milli>(Zirc::GameBoy::MS_PER_FRAME) - frameMs);
		}
	}

	void drawPixel(uint8_t x, uint8_t y, Zirc::Color color) override { m_videoBackend->drawPixel(x, y, color); }

	void showFrame() override { m_videoBackend->queueRender(); }

	void swapBuffers() override {
		m_videoBackend->swapBuffers();
		Zirc::Config::get().activeColorPalette = m_nextActiveColorPalette;
	}

	float getAudioAmplitude() const override { return 1.0; }
	float getAudioSampleRate() const override { return m_audioBackend->audioSampleRate(); }
	void pushAudioSample(float sample) override { m_audioBackend->pushSample(sample); }
	void muteAudio() override { m_audioBackend->pause(); }
	void unmuteAudio() override { m_audioBackend->unPause(); }
	void resetAudio() { m_audioBackend->restart(); }

	void handleKeyPressed(guint keyVal) { handleKey(keyVal, true); }
	void handleKeyReleased(guint keyVal) { handleKey(keyVal, false); }

	void requestOpenPreferencesWindow() {
		if(m_preferencesWindow) {
			std::cout << "Already opened" << std::endl;
			return;
		}

		m_preferencesWindow = std::make_unique<PreferencesWindow>(*this);
		m_preferencesWindow->set_transient_for(*this);
		m_preferencesWindow->signal_close_request().connect(
			[this]() -> bool {
				Glib::signal_idle().connect_once([this]() { m_preferencesWindow.reset(); });
				return true;
			},
			false);
		m_preferencesWindow->show();
	}

	void updateCustomBootRom() {
		if(!m_gameBoy) { return; }
		if(!Zirc::Config::get().useCustomBootRom) {
			m_gameBoy->disableCustomBootRom();
			return;
		}

		try {
			m_gameBoy->loadCustomBootRom(Zirc::Config::get().customBootRomPath);
		} catch(const std::runtime_error &e) {
			std::cout << "[WARNING] The custom boot ROM file was not loaded: \"" << e.what() << "\"" << std::endl;
		}
	}

  private:
	void setupWindow();
	void setupKeyController();

	bool tick() {
		if(!m_gameBoy) { return true; }

		while(Zirc::Config::get().skipIntro && !m_gameBoy->introEnded()) {
			m_gameBoy->tick();
		}

		int cycles = 0;
		while(cycles < Zirc::GameBoy::CYCLES_PER_FRAME) {
			cycles += m_gameBoy->tick();
		}

		showFrame();
		return true;
	}

	void handleKey(guint keyVal, bool pressed) {
		if(!m_gameBoy) { return; }

		Zirc::Joypad::Key key;
		switch(keyVal) {
		case GDK_KEY_0: m_nextActiveColorPalette = 0; return;
		case GDK_KEY_1: m_nextActiveColorPalette = 1; return;
		case GDK_KEY_2: m_nextActiveColorPalette = 2; return;
		case GDK_KEY_Up: key = Zirc::Joypad::Key::Up; break;
		case GDK_KEY_Down: key = Zirc::Joypad::Key::Down; break;
		case GDK_KEY_Left: key = Zirc::Joypad::Key::Left; break;
		case GDK_KEY_Right: key = Zirc::Joypad::Key::Right; break;
		case GDK_KEY_Return: key = Zirc::Joypad::Key::Start; break;
		case GDK_KEY_BackSpace: key = Zirc::Joypad::Key::Select; break;
		case GDK_KEY_a: key = Zirc::Joypad::Key::A; break;
		case GDK_KEY_b: key = Zirc::Joypad::Key::B; break;
		default: return;
		}

		if(pressed) {
			m_gameBoy->handleKeydown(key);
		} else {
			m_gameBoy->handleKeyup(key);
		}
	}

	uint8_t m_nextActiveColorPalette = Zirc::Config::get().activeColorPalette;

	bool m_running = true;

	Gtk::Label m_noRomLabel{"There is no ROM loaded"};

	Zirc::GameBoy *m_gameBoy = nullptr;

	GtkMenuBar::MenuBar m_menuBar;
	Gtk::Box m_contentBox;

	std::unique_ptr<VideoBackend> m_videoBackend = nullptr;
	std::unique_ptr<AudioBackend> m_audioBackend = nullptr;

	std::shared_ptr<PreferencesWindow> m_preferencesWindow = nullptr;
};
