#pragma once

#include <chrono>
#include <cstring>
#include <gtkmm.h>
#include <ratio>
#include <thread>

#include "gameboy.hpp"
#include "joypad.hpp"
#include "platform.hpp"

class PlatformGtk : public Platform {
  public:
	PlatformGtk() {
		m_drawingArea.set_vexpand(true);
		m_drawingArea.set_hexpand(true);
		m_drawingArea.set_size_request(Lcd::WIDTH, Lcd::HEIGHT);

		m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::RGB24, Lcd::WIDTH, Lcd::HEIGHT);
		m_drawingArea.set_draw_func(sigc::mem_fun(*this, &PlatformGtk::frameDrawCallback));
	}

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

	float getAudioAmplitude() const override { return 1; }
	float getAudioSampleRate() const override { return 44100; }
	void pushAudioSample(float sample) override {}
	void muteAudio() override {}
	void unmuteAudio() override {}

	Gtk::DrawingArea &getDrawingArea() { return m_drawingArea; }

	void initializeBackBuffer() { std::memset(m_backBuffer, 0, m_surface->get_stride() * Lcd::HEIGHT); }

	void handleKeyPressed(guint keyVal) { handleKey(keyVal, true); }
	void handleKeyReleased(guint keyVal) { handleKey(keyVal, false); }

  private:
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
};
