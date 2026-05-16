#pragma once

#include <cstdint>
#include <string>

namespace Config {
extern bool limitFps;
extern bool debugOutput;
extern bool serialOutput;
extern bool skipIntro;

extern bool useCustomBootRom;
extern std::string customBootRomPath;
extern uint8_t gtkVideoBackend; // 0 -> OpenGL, 1 -> GTK/software
extern uint8_t gtkAudioBackend; // 0 -> PortAudio, 1 -> SDL2
}; // namespace Config
