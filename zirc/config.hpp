#pragma once

#include <cstdint>
#include <string>

namespace Zirc {
class Config {
  public:
	Config(Config &) = delete;
	void operator=(const Config &) = delete;

	static Config &get() {
		static Config config;
		return config;
	}

	bool debugOutput = false;
	bool serialOutput = false;
	bool skipIntro = false;

	uint8_t activeColorPalette = 0;

	bool useCustomBootRom = false;
	std::string customBootRomPath{""};

  private:
	Config() = default;
};
} // namespace Zirc
