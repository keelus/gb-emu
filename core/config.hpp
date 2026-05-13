#pragma once


#include <string>

namespace Config {
inline bool limitFps = true;
inline bool debugOutput = false;
inline bool serialOutput = false;
inline bool skipIntro = false;

inline bool useCustomBootRom = false;
inline std::string customBootRomPath{""};
}; // namespace Config
