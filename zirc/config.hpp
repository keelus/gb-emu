#pragma once

#include <string>

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

	bool useCustomBootRom = false;
	std::string customBootRomPath{""};

  private:
	Config() = default;
}; // namespace Config
