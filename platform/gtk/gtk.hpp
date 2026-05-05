#pragma once

#include <chrono>
#include <cstring>
#include <gtkmm.h>
#include <gtkmm/window.h>
#include <portaudio.h>
#include <ratio>
#include <thread>

#include "audio/ringbuffer.hpp"
#include "config.hpp"
#include "gameboy.hpp"
#include "joypad.hpp"
#include "menu_bar/menu_bar.hpp"
#include "platform.hpp"

class PlatformGtk : public Platform, public Gtk::Window {
  public:
	PlatformGtk(int argc, char *argv[]) : m_menuBar(*this) {
		const char *audioErrorMsg = setupAudio();
		if(audioErrorMsg) {
			auto dialog = Gtk::AlertDialog::create();
			dialog->set_message("PortAudio error");
			dialog->set_detail("Audio couldn't be initialized: \"" + std::string(audioErrorMsg) + "\"");
			dialog->set_modal(true);
			dialog->show();
		}

		setupWindow();
	}

	~PlatformGtk() {
		if(m_stream) {
			Pa_StopStream(m_stream);
			Pa_CloseStream(m_stream);
		}
		Pa_Terminate();
	}

	void addGameBoy(GameBoy *gameBoy, const std::string &romName);
	void resetGameBoy();
	void removeGameBoy();

	void frameDrawCallback(const Cairo::RefPtr<Cairo::Context> &cr, int w, int h) {
		m_surface->flush();

		unsigned char *pixels = m_surface->get_data();
		int stride = m_surface->get_stride();

		m_surface->mark_dirty();

		cr->set_source_rgb(0, 0, 0);
		cr->paint();

		double size = std::min(w, h);
		double offsetX = (w - size) / 2.0;
		double offsetY = (h - size) / 2.0;

		cr->translate(offsetX, offsetY);
		cr->scale(size / Lcd::WIDTH, size / Lcd::HEIGHT);

		auto pattern = Cairo::SurfacePattern::create(m_surface);
		pattern->set_filter(Cairo::SurfacePattern::Filter::NEAREST);
		cr->set_source(pattern);
		cr->paint();
	}

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

	void drawPixel(uint8_t x, uint8_t y, Color color) override {
		char *px = m_backBuffer + y * m_surface->get_stride() + x * 4;
		px[0] = color.blue();
		px[1] = color.green();
		px[2] = color.red();
	}

	void showFrame() override { m_drawingArea.queue_draw(); }
	void swapBuffers() override {
		std::memcpy(m_surface->get_data(), m_backBuffer, sizeof(unsigned char) * m_surface->get_stride() * Lcd::HEIGHT);
		std::memset(m_backBuffer, 0, sizeof(unsigned char) * m_surface->get_stride() * Lcd::HEIGHT);
	}

	static int audioCallback(const void *input, void *output, unsigned long framesPerBuffer,
							 const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags,
							 void *userData) {
		PlatformGtk *platform = static_cast<PlatformGtk *>(userData);
		float *buffer = (float *)output;

		for(size_t i = 0; i < framesPerBuffer; i++) {
			if(platform->m_audioPaused || !platform->m_audioSampleBuffer.popSample(buffer[i])) { buffer[i] = 0.0f; }
		}

		return paContinue;
	}
	float getAudioAmplitude() const override { return 1.0; }
	float getAudioSampleRate() const override { return AUDIO_SAMPLE_RATE; }
	void pushAudioSample(float sample) override { m_audioSampleBuffer.pushSample(sample); }
	void muteAudio() override { m_audioPaused = true; }
	void unmuteAudio() override { m_audioPaused = false; }
	void resetAudio() { m_audioSampleBuffer.reset(); }

	void handleKeyPressed(guint keyVal) { handleKey(keyVal, true); }
	void handleKeyReleased(guint keyVal) { handleKey(keyVal, false); }

  private:
	const char *setupAudio();
	void setupWindow();
	void setupDrawingArea();
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
		case GDK_KEY_0: activeColorPalette = 0; return;
		case GDK_KEY_1: activeColorPalette = 1; return;
		case GDK_KEY_2: activeColorPalette = 2; return;
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

	bool m_running = true;

	Gtk::DrawingArea m_drawingArea;
	Cairo::RefPtr<Cairo::ImageSurface> m_surface;
	char m_backBuffer[Lcd::WIDTH * Lcd::HEIGHT * 4];

	PaStream *m_stream = nullptr;
	bool m_audioPaused = true;
	AudioRingBuffer<4096> m_audioSampleBuffer;
	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr size_t AUDIO_SAMPLE_AMOUNT = 1024;

	Gtk::Label m_noRomLabel{"There is no ROM loaded"};

	GameBoy *m_gameBoy = nullptr;

	GtkMenuBar::MenuBar m_menuBar;
};
