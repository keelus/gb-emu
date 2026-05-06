#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
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
#include "graphics/background_fifo.hpp"
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
		GLubyte *px = m_backBuffer + (Lcd::HEIGHT - 1 - y) * Lcd::WIDTH * 3 + x * 3;
		px[0] = color.red();
		px[1] = color.green();
		px[2] = color.blue();
	}

	void showFrame() override { m_glArea.queue_render(); }
	void swapBuffers() override {
		std::memcpy(m_frontBuffer, m_backBuffer, sizeof(m_backBuffer));
		std::memset(m_backBuffer, 0, sizeof(m_backBuffer));
		activeColorPalette = m_nextActiveColorPalette;
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

	bool glDraw(const Glib::RefPtr<Gdk::GLContext> &context) {
		glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Lcd::WIDTH, Lcd::HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, m_frontBuffer);

		glUseProgram(m_shaderProgram);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		return true;
	}

	void realizeOpenGl();

  private:
	const char *setupAudio();
	void setupWindow();
	void setupOpenGl();
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

	Gtk::GLArea m_glArea;

	unsigned int m_shaderProgram, m_VAO;
	// clang-format off
	float m_vertices[32] = {
		-1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
		1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		1.0f,  1.0f,  0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
	};
	unsigned int m_indices[6] = {
	    0, 1, 2,
		0, 2, 3
	};
	// clang-format on

	unsigned int m_texture;
	GLubyte m_backBuffer[Lcd::WIDTH * Lcd::HEIGHT * 3] = {0};
	GLubyte m_frontBuffer[Lcd::WIDTH * Lcd::HEIGHT * 3] = {0};

	PaStream *m_stream = nullptr;
	bool m_audioPaused = true;
	AudioRingBuffer<4096> m_audioSampleBuffer;
	static constexpr float AUDIO_SAMPLE_RATE = 44100.0;
	static constexpr size_t AUDIO_SAMPLE_AMOUNT = 1024;

	Gtk::Label m_noRomLabel{"There is no ROM loaded"};

	GameBoy *m_gameBoy = nullptr;

	GtkMenuBar::MenuBar m_menuBar;
};
