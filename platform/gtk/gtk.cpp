#include <gtkmm.h>
#include <thread>

#include "gtk.hpp"
#include "emulator.hpp"

class MyWindow : public Gtk::Window {
  public:
	MyWindow(int argc, char *argv[]) {
		set_title("Gb-emu");
		set_default_size(512, 512);

		m_platform.initializeBackBuffer();

		auto keyController = Gtk::EventControllerKey::create();
		keyController->signal_key_pressed().connect(
			[this](guint keyVal, guint, Gdk::ModifierType) -> bool {
				m_platform.handleKeyPressed(keyVal);
				return true;
			},
			false);
		keyController->signal_key_released().connect(
			[this](guint keyVal, guint, Gdk::ModifierType) { m_platform.handleKeyReleased(keyVal); }, false);
		add_controller(keyController);

		set_child(m_platform.getDrawingArea());

		m_emulatorThread = std::thread([this, argc, argv]() {
			Emulator emulator(m_platform);
			int result = emulator.run(argc, argv);
			Glib::signal_idle().connect_once([result]() { exit(result); });
		});
	}

	~MyWindow() {
		if(m_emulatorThread.joinable()) {
			m_platform.stop();
			m_emulatorThread.join();
		}
	}

  private:
	PlatformGtk m_platform;

	std::thread m_emulatorThread;
	Emulator *m_emulator;
};

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create("org.gtkmm.examples.base");
	return app->make_window_and_run<MyWindow>(0, NULL, argc, argv);
}
