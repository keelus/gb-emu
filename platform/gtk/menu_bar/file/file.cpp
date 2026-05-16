#include <cassert>
#include <filesystem>
#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <giomm/asyncresult.h>
#include <glibmm/refptr.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/error.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/label.h>
#include <gtkmm/object.h>
#include <gtkmm/popovermenubar.h>

#include "gtk.hpp"

namespace GtkMenuBar {
File::File(PlatformGtk &platform) : m_platform(platform) {
	m_menu = Gio::Menu::create();
	appendRomSection();
	appendSaveGameSection();
	appendDebugSection();
	appendExitSection();
}

void File::appendRomSection() {
	auto romSection = Gio::Menu::create();
	romSection->append("Open ROM", "fileRom.openRom");
	romSection->append("Reset ROM", "fileRom.resetRom");
	romSection->append("Close ROM", "fileRom.closeRom");
	m_menu->append_section({}, romSection);

	auto actionGroup = Gio::SimpleActionGroup::create();
	actionGroup->add_action("openRom", [this]() {
		auto dialog = Gtk::FileDialog::create();
		dialog->set_title("Select a ROM file");

		dialog->open(m_platform, [this, dialog](Glib::RefPtr<Gio::AsyncResult> &result) {
			Zirc::GameBoy *gameBoy = nullptr;
			std::filesystem::path romPath;
			try {
				auto file = dialog->open_finish(result);
				romPath = std::filesystem::path(file->get_path());

				gameBoy = new Zirc::GameBoy(romPath, m_platform);
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
				m_platform.addGameBoy(gameBoy, romPath.filename());
				m_platform.updateCustomBootRom();

				m_romResetAction->set_enabled(true);
				m_romCloseAction->set_enabled(true);
			}
		});
	});

	m_romResetAction = Gio::SimpleAction::create("resetRom");
	m_romResetAction->signal_activate().connect([this](const Glib::VariantBase &) { m_platform.resetGameBoy(); });
	actionGroup->add_action(m_romResetAction);
	m_romResetAction->set_enabled(false);

	m_romCloseAction = Gio::SimpleAction::create("closeRom");
	m_romCloseAction->signal_activate().connect([this](const Glib::VariantBase &) {
		m_platform.removeGameBoy();

		m_romResetAction->set_enabled(false);
		m_romCloseAction->set_enabled(false);
	});
	actionGroup->add_action(m_romCloseAction);
	m_romCloseAction->set_enabled(false);

	m_platform.insert_action_group("fileRom", actionGroup);
}

void File::appendSaveGameSection() {
	auto saveGameSection = Gio::Menu::create();

	auto saveGameSubMenu = Gio::Menu::create();
	saveGameSubMenu->append("Load from file", "fileSaveGame.openSaveGame");
	saveGameSubMenu->append("Save to a file", "fileSaveGame.saveGame");

	saveGameSection->append_submenu("Save Game", saveGameSubMenu);
	m_menu->append_section({}, saveGameSection);
}

void File::appendDebugSection() {
	auto debugSection = Gio::Menu::create();

	auto stateSubMenu = Gio::Menu::create();
	stateSubMenu->append("Load from file", "fileDebug.loadState");
	stateSubMenu->append("Dump to a file", "fileDebug.saveState");

	auto exportSubMenu = Gio::Menu::create();
	exportSubMenu->append("Dump RAM", "fileDebug.dumpRam");
	exportSubMenu->append("Dump VRAM", "fileDebug.dumpVram");

	debugSection->append_submenu("State", stateSubMenu);
	debugSection->append_submenu("Export", exportSubMenu);
	m_menu->append_section({}, debugSection);
}

void File::appendExitSection() {
	auto exitSection = Gio::Menu::create();
	exitSection->append("Preferences", "fileExit.preferences");
	exitSection->append("Exit", "fileExit.exit");
	m_menu->append_section({}, exitSection);

	auto actionGroup = Gio::SimpleActionGroup::create();
	actionGroup->add_action("preferences", [this]() { m_platform.requestOpenPreferencesWindow(); });
	actionGroup->add_action("exit", [this]() { m_platform.close(); });
	m_platform.insert_action_group("fileExit", actionGroup);
}

}; // namespace GtkMenuBar
