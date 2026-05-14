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
}; // namespace Config
