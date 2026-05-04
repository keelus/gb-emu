#include <cstdint>
#include <filesystem>
#include <giomm/asyncresult.h>
#include <giomm/simpleaction.h>
#include <giomm/simpleactiongroup.h>
#include <glibmm/main.h>
#include <gtkmm.h>
#include <gtkmm/box.h>
#include <gtkmm/dialog.h>
#include <gtkmm/enums.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <gtkmm/orientable.h>
#include <gtkmm/popovermenubar.h>
#include <memory>
#include <thread>

#include "config.hpp"
#include "gameboy.hpp"
#include "gtk.hpp"

uint8_t activeColorPalette = 0;

class MyWindow : public Gtk::Window {
  public:
	MyWindow(int argc, char *argv[]) {
		set_title("Zirc Emulator");
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


		m_noRomLabel.set_hexpand(true);
		m_noRomLabel.set_vexpand(true);

		auto menubar = Gio::Menu::create();
		{
			auto fileMenu = Gio::Menu::create();

			auto romSection = Gio::Menu::create();
			romSection->append("Open ROM", "app.openRom");
			romSection->append("Reset", "app.reset");
			romSection->append("Close ROM", "app.closeRom");
			fileMenu->append_section({}, romSection);

			menubar->append_submenu("File", fileMenu);
		}

		Glib::RefPtr<Gio::MenuModel> model = menubar;

		auto actionGroup = Gio::SimpleActionGroup::create();

		actionGroup->add_action("openRom", [this]() {
			auto dialog = Gtk::FileDialog::create();
			dialog->set_title("Select a ROM file");

			dialog->open(*this, [this, dialog](Glib::RefPtr<Gio::AsyncResult> &result) {
				GameBoy *gameBoy = nullptr;
				std::filesystem::path romPath;
				try {
					auto file = dialog->open_finish(result);
					romPath = std::filesystem::path(file->get_path());

					gameBoy = new GameBoy(romPath, m_platform);
				} catch(const Gtk::DialogError &e) {

				} catch(const std::runtime_error &e) {
					if(gameBoy) { delete gameBoy; }

					auto dialog = Gtk::AlertDialog::create();
					dialog->set_message("Error");
					dialog->set_detail(e.what());
					dialog->set_modal(true);
					dialog->show();
				}

				if(gameBoy) {
					if(m_gameBoy) { delete m_gameBoy; }
					m_gameBoy = gameBoy;
					m_platform.addGameBoy(gameBoy);
					m_platform.muteAudio();
					m_platform.resetAudio();
					set_title("Zirc Emulator - " + std::string(romPath.filename()));

					m_resetAction->set_enabled(true);
					m_romCloseAction->set_enabled(true);

					m_platform.getDrawingArea().set_visible(true);
					m_noRomLabel.set_visible(false);
				}
			});
		});

		m_resetAction = Gio::SimpleAction::create("reset");
		m_resetAction->signal_activate().connect([this](const Glib::VariantBase &) {
			m_gameBoy->reset();

			m_platform.muteAudio();
			m_platform.resetAudio();
		});
		actionGroup->add_action(m_resetAction);
		m_resetAction->set_enabled(false);

		m_romCloseAction = Gio::SimpleAction::create("closeRom");
		m_romCloseAction->signal_activate().connect([this](const Glib::VariantBase &) {
			m_platform.removeGameBoy();
			delete m_gameBoy;
			m_gameBoy = nullptr;

			set_title("Zirc Emulator");
			m_resetAction->set_enabled(false);
			m_romCloseAction->set_enabled(false);

			m_platform.muteAudio();
			m_platform.resetAudio();

			m_platform.getDrawingArea().set_visible(false);
			m_noRomLabel.set_visible(true);
		});
		actionGroup->add_action(m_romCloseAction);
		m_romCloseAction->set_enabled(false);

		insert_action_group("app", actionGroup);

		auto menuBar = Gtk::make_managed<Gtk::PopoverMenuBar>(model);


		Gtk::Box box;
		box.set_orientation(Gtk::Orientation::VERTICAL);
		box.append(*menuBar);
		box.append(m_platform.getDrawingArea());
		box.append(m_noRomLabel);

		m_platform.getDrawingArea().set_visible(false);

		set_child(box);

		Glib::signal_timeout().connect(sigc::mem_fun(*this, &MyWindow::tick), 16);
	}

	~MyWindow() {
		if(m_emulatorThread.joinable()) {
			m_platform.stop();
			m_emulatorThread.join();
		}
	}

	bool tick() {
		if(!m_gameBoy) { return true; }

		while(Config::skipIntro && !m_gameBoy->introEnded()) {
			m_gameBoy->tick();
		}

		int cycles = 0;
		while(cycles < GameBoy::CYCLES_PER_FRAME) {
			cycles += m_gameBoy->tick();
		}

		m_platform.showFrame();
		return true;
	}

  private:
	Gtk::Label m_noRomLabel{"There is no ROM loaded"};

	PlatformGtk m_platform;

	GameBoy *m_gameBoy = nullptr;

	std::thread m_emulatorThread;

	Glib::RefPtr<Gio::SimpleAction> m_resetAction = nullptr;
	Glib::RefPtr<Gio::SimpleAction> m_romCloseAction = nullptr;
};

int main(int argc, char *argv[]) {
	auto app = Gtk::Application::create("org.keelus.zirc");
	return app->make_window_and_run<MyWindow>(0, NULL, argc, argv);
}
