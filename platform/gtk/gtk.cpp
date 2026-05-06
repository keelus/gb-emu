#include "gtk.hpp"

void PlatformGtk::addGameBoy(GameBoy *gameBoy, const std::string &romName) {
	if(m_gameBoy) { delete m_gameBoy; }
	m_gameBoy = gameBoy;

	muteAudio();
	resetAudio();

	set_title("Zirc Emulator - " + romName);

	m_glArea.set_visible();
	m_noRomLabel.set_visible(false);
}

void PlatformGtk::resetGameBoy() {
	muteAudio();
	resetAudio();

	m_gameBoy->reset();
}

void PlatformGtk::removeGameBoy() {
	muteAudio();
	resetAudio();

	delete m_gameBoy;
	m_gameBoy = nullptr;

	m_glArea.set_visible(false);
	m_noRomLabel.set_visible();

	set_title("Zirc Emulator");
}
