#include <gtkmm/application.h>

#include "gtk.hpp"

uint8_t activeColorPalette = 0;

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create("org.keelus.zirc");
	return app->make_window_and_run<PlatformGtk>(0, NULL, argc, argv);
}
