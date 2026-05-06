#include <filesystem>
#include <giomm/menu.h>
#include <giomm/simpleactiongroup.h>
#include <giomm/asyncresult.h>
#include <glibmm/refptr.h>
#include <gtkmm/alertdialog.h>
#include <gtkmm/error.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/label.h>
#include <gtkmm/popovermenubar.h>

class PlatformGtk;

namespace GtkMenuBar {
class File {
  public:
	File(PlatformGtk &platform);

	Glib::RefPtr<Gio::Menu> getMenu() { return m_menu; }

  private:
	void appendRomSection();
	void appendSaveGameSection();
	void appendDebugSection();
	void appendExitSection();

	Glib::RefPtr<Gio::Menu> m_menu = nullptr;

	Glib::RefPtr<Gio::SimpleAction> m_romResetAction = nullptr;
	Glib::RefPtr<Gio::SimpleAction> m_romCloseAction = nullptr;

	PlatformGtk &m_platform;
};
} // namespace GtkMenuBar
