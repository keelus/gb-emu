#include "config.hpp"

namespace Config {
bool limitFps = true;
bool debugOutput = false;
bool serialOutput = false;
bool skipIntro = false;

bool useCustomBootRom = false;
std::string customBootRomPath{""};
uint8_t gtkVideoBackend = 0; // 0 -> OpenGL, 1 -> GTK/software
uint8_t gtkAudioBackend = 0; // 0 -> PortAudio, 1 -> SDL2
} // namespace Config
