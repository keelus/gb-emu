#pragma once

#include <gtkmm/box.h>
#include <gtkmm/enums.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>
#include <gtkmm/listbox.h>
#include <gtkmm/listboxrow.h>
#include <gtkmm/object.h>
#include <gtkmm/orientable.h>
#include <gtkmm/stacksidebar.h>
#include <gtkmm/stackswitcher.h>
#include <gtkmm/window.h>
class PreferencesWindow : public Gtk::Window {
  public:
	PreferencesWindow() {
		set_title("Preferences - Zirc Emulator");
		set_default_size(512, 512);

		auto sideBar = Gtk::make_managed<Gtk::StackSidebar>();
		auto stack = Gtk::make_managed<Gtk::Stack>();

		Gtk::Label label{"Audio settings go here"};
		auto page1 = stack->add(label, "audio", "Audio");
		Gtk::Label label2{"Video settings go here"};
		auto page2 = stack->add(label2, "video", "Video");

		sideBar->set_stack(*stack);

		auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);
		vbox->append(*sideBar);
		vbox->append(*stack);

		set_child(*vbox);
	}
};
