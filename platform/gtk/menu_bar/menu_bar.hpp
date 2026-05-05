#include <gtkmm/popovermenubar.h>
#include "file/file.hpp"

class PlatformGtk;

namespace GtkMenuBar {
class MenuBar {
  public:
	class File;

	MenuBar(PlatformGtk &platform) : m_menuBar(), m_file(platform) {
		auto menu = Gio::Menu::create();

		menu->append_submenu("File", m_file.getMenu());

		m_menuBar = Gtk::make_managed<Gtk::PopoverMenuBar>(menu);
	}

	Gtk::PopoverMenuBar *getMenuBar() { return m_menuBar; }

  private:
	Gtk::PopoverMenuBar *m_menuBar = nullptr;
	GtkMenuBar::File m_file;
};
} // namespace GtkMenuBar
