#pragma once

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/checkbutton.h>
#include <gtkmm/entry.h>
#include <gtkmm/enums.h>
#include <gtkmm/error.h>
#include <gtkmm/filechooser.h>
#include <gtkmm/filechooserdialog.h>
#include <gtkmm/filedialog.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/object.h>
#include <gtkmm/orientable.h>
#include <gtkmm/separator.h>
#include <gtkmm/stacksidebar.h>
#include <gtkmm/stackswitcher.h>
#include <gtkmm/text.h>
#include <gtkmm/window.h>

class PlatformGtk;

class PreferencesWindow : public Gtk::Window {
  public:
	PreferencesWindow(PlatformGtk &platform);

  private:
	Gtk::CheckButton m_bootImageUseBuiltin, m_bootImageUseCustom;

	Gtk::Entry m_bootImageCustomTextField;
	Gtk::Button m_bootImageCustomBrowseButton;

	Gtk::CheckButton m_skipBootCheckButton;

	PlatformGtk &m_platform;
};
