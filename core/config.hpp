#pragma once


#include <cstdint>
#include <string>

namespace Config {
inline bool limitFps = true;
inline bool debugOutput = false;
inline bool serialOutput = false;
inline bool skipIntro = false;

inline bool useCustomBootRom = false;
inline std::string customBootRomPath{""};
inline uint8_t gtkVideoBackend = 0; // 0 -> OpenGL, 1 -> GTK/software
inline uint8_t gtkAudioBackend = 0; // 0 -> PortAudio, 1 -> SDL2
}; // namespace Config
