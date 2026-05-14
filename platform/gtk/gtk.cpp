#include "gtk.hpp"

void PlatformGtk::setupWindow() {
	set_title("Zirc Emulator");
	set_default_size(512, 512);

	setupKeyController();

	set_child(m_contentBox);
	m_contentBox.set_orientation(Gtk::Orientation::VERTICAL);

	m_contentBox.append(*m_menuBar.getMenuBar());

	m_contentBox.append(m_noRomLabel);
	m_noRomLabel.set_hexpand();
	m_noRomLabel.set_vexpand();

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &PlatformGtk::tick), 16);
}

void PlatformGtk::setupKeyController() {
	auto keyController = Gtk::EventControllerKey::create();

	keyController->signal_key_pressed().connect(
		[this](guint keyVal, guint, Gdk::ModifierType) -> bool {
			handleKeyPressed(keyVal);
			return true;
		},
		false);
	keyController->signal_key_released().connect(
		[this](guint keyVal, guint, Gdk::ModifierType) { handleKeyReleased(keyVal); }, false);

	add_controller(keyController);
}

void PlatformGtk::addGameBoy(GameBoy *gameBoy, const std::string &romName) {
	if(m_gameBoy) { delete m_gameBoy; }
	m_gameBoy = gameBoy;

	muteAudio();
	resetAudio();

	set_title("Zirc Emulator - " + romName);

	m_videoBackend->setVisible(true);
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

	m_videoBackend->setVisible(false);
	m_noRomLabel.set_visible();

	set_title("Zirc Emulator");
}
