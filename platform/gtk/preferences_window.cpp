
#include "config.hpp"
#include "gtk.hpp"
#include <giomm/listmodel.h>
#include <gtkmm/dropdown.h>
#include <gtkmm/stringlist.h>

PreferencesWindow::PreferencesWindow(PlatformGtk &platform) : m_platform(platform) {
	set_title("Preferences - Zirc Emulator");
	set_default_size(512, 512);

	auto sideBar = Gtk::make_managed<Gtk::StackSidebar>();
	auto stack = Gtk::make_managed<Gtk::Stack>();

	Gtk::Label label{"Audio settings go here"};
	auto page1 = stack->add(label, "audio", "Audio");

	Gtk::Box videoPanel;
	videoPanel.set_hexpand();
	videoPanel.set_vexpand();
	videoPanel.set_orientation(Gtk::Orientation::VERTICAL);

	std::vector<Glib::ustring> strings{"OpenGL", "GTK (software)"};
	auto options = Gtk::StringList::create(strings);
	m_videoBackendDropdown.set_model(options);
	videoPanel.append(m_videoBackendDropdown);
	m_videoBackendDropdown.property_selected().signal_changed().connect([this]() {
		Config::gtkVideoBackend = m_videoBackendDropdown.get_selected();
		m_platform.reloadVideoBackend();
	});
	auto page2 = stack->add(videoPanel, "video", "Video");

	Gtk::Box bootPanel;
	bootPanel.set_hexpand();
	bootPanel.set_vexpand();
	bootPanel.set_orientation(Gtk::Orientation::VERTICAL);

	Gtk::Label bootImageTitle{"Boot image to use"};
	bootPanel.append(bootImageTitle);

	m_bootImageUseBuiltin.set_label("Use builtin boot image.");
	m_bootImageUseBuiltin.set_active(!Config::useCustomBootRom);

	m_bootImageUseCustom.set_label("Use custom boot image (if file is found and valid).");
	m_bootImageUseCustom.set_active(Config::useCustomBootRom);
	m_bootImageUseCustom.set_group(m_bootImageUseBuiltin);

	m_bootImageUseBuiltin.signal_toggled().connect([this]() {
		Config::useCustomBootRom = !m_bootImageUseBuiltin.get_active();
		m_platform.updateCustomBootRom();

		m_bootImageCustomBrowseButton.set_sensitive(Config::useCustomBootRom);
		m_bootImageCustomTextField.set_editable(Config::useCustomBootRom);
	});

	bootPanel.append(m_bootImageUseBuiltin);
	bootPanel.append(m_bootImageUseCustom);

	Gtk::Box customFileBox;
	customFileBox.set_hexpand();
	customFileBox.set_margin_end(5);
	customFileBox.set_margin_bottom(5);

	m_bootImageCustomTextField.set_editable(Config::useCustomBootRom);
	m_bootImageCustomTextField.set_hexpand();
	m_bootImageCustomTextField.set_margin_end(5);
	m_bootImageCustomTextField.set_text(Config::customBootRomPath);
	customFileBox.append(m_bootImageCustomTextField);

	m_bootImageCustomBrowseButton.set_label("Browse");
	m_bootImageCustomBrowseButton.set_sensitive(Config::useCustomBootRom);
	m_bootImageCustomBrowseButton.signal_clicked().connect([this]() {
		auto fileChooser = Gtk::FileDialog::create();
		fileChooser->set_modal();
		fileChooser->open(*this, [this, fileChooser](Glib::RefPtr<Gio::AsyncResult> &result) {
			try {
				auto file = fileChooser->open_finish(result);
				Config::customBootRomPath = file->get_path();
				m_bootImageCustomTextField.set_text(Config::customBootRomPath);
				m_platform.updateCustomBootRom();
			} catch(const Gtk::DialogError &e) { std::cout << "FileDialog error: " << e.what() << std::endl; }
		});
	});
	customFileBox.append(m_bootImageCustomBrowseButton);

	bootPanel.append(customFileBox);

	Gtk::Separator sep;
	bootPanel.append(sep);

	m_skipBootCheckButton.set_active(Config::skipIntro);
	m_skipBootCheckButton.set_label("Skip boot intro");
	m_skipBootCheckButton.signal_toggled().connect(
		[this]() { Config::skipIntro = m_skipBootCheckButton.get_active(); });
	bootPanel.append(m_skipBootCheckButton);

	auto page3 = stack->add(bootPanel, "boot", "Boot");

	sideBar->set_stack(*stack);

	auto vbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 5);
	vbox->append(*sideBar);
	vbox->append(*stack);

	set_child(*vbox);
}
