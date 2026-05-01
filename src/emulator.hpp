#pragma once

#include <cassert>
#include <unistd.h>

#include "platform.hpp"

class Emulator {
  public:
	Emulator(Platform &platform) : m_platform(platform) {}
	int run(int argc, char *argv[]);

  private:
	void printUsage(const char *argv0, const bool isHelpMessage);

	Platform &m_platform;
};
