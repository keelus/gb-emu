#pragma once

#include <cstdint>

class GtkConfig {
  public:
	GtkConfig(GtkConfig &) = delete;
	void operator=(const GtkConfig &) = delete;

	static GtkConfig &get() {
		static GtkConfig config;
		return config;
	}

	uint8_t videoBackend = 0; // 0 -> OpenGL, 1 -> GTK/software
	uint8_t audioBackend = 0; // 0 -> PortAudio, 1 -> SDL2

  private:
	GtkConfig() = default;
}; // namespace Config
