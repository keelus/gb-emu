#pragma once

#include <chrono>
#include <cstring>
#include <gtkmm.h>
#include <portaudio.h>
#include <ratio>
#include <thread>

#include "audio/ringbuffer.hpp"
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

		PaError err = Pa_Initialize();
		if(err != paNoError) {
			std::cerr << "Pa_Initialize() error: " << Pa_GetErrorText(err) << std::endl;
			return;
		}

		err = Pa_OpenDefaultStream(&m_stream, 0, 1, paFloat32, AUDIO_SAMPLE_RATE, AUDIO_SAMPLE_AMOUNT, audioCallback,
								   this);
		if(err != paNoError) {
			std::cerr << "Pa_OpenDefaultStream() error: " << Pa_GetErrorText(err) << std::endl;
			return;
		}

		err = Pa_StartStream(m_stream);
		if(err != paNoError) {
			std::cerr << "Pa_StartStream() error: " << Pa_GetErrorText(err) << std::endl;
			return;
		}
	}
	~PlatformGtk() {
		if(m_stream) { Pa_CloseStream(m_stream); }
		Pa_Terminate();
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
			if(!platform->m_audioSampleBuffer.popSample(buffer[i]) || platform->m_audioPaused) { buffer[i] = 0.0f; }
		}

		return paContinue;
	}

	float getAudioAmplitude() const override { return 1.0; }
	float getAudioSampleRate() const override { return AUDIO_SAMPLE_RATE; }
	void pushAudioSample(float sample) override { m_audioSampleBuffer.pushSample(sample); }
	void muteAudio() override { m_audioPaused = true; }
	void unmuteAudio() override { m_audioPaused = false; }

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

	PaStream *m_stream = nullptr;
	bool m_audioPaused = true;
	AudioRingBuffer<4096> m_audioSampleBuffer;
	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr size_t AUDIO_SAMPLE_AMOUNT = 1024;
};
