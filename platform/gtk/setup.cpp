#include <gdkmm/glcontext.h>

#include "gtk.hpp"

void PlatformGtk::setupWindow() {
	set_title("Zirc Emulator");
	set_default_size(512, 512);

	setupKeyController();

	Gtk::Box box;
	set_child(box);
	box.set_orientation(Gtk::Orientation::VERTICAL);

	box.append(*m_menuBar.getMenuBar());

	box.append(*m_openGlSubsystem.getGlArea());
	m_openGlSubsystem.getGlArea()->set_visible(false);

	box.append(m_noRomLabel);
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
