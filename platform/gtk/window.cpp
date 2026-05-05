#include <filesystem>

#include "gtk.hpp"
#include "portaudio.h"

void PlatformGtk::setupWindow() {
	set_title("Zirc Emulator");
	set_default_size(512, 512);

	setupDrawingArea();
	setupKeyController();

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

				gameBoy = new GameBoy(romPath, *this);
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
				addGameBoy(gameBoy);
				muteAudio();
				resetAudio();
				set_title("Zirc Emulator - " + std::string(romPath.filename()));

				m_resetAction->set_enabled(true);
				m_romCloseAction->set_enabled(true);

				m_drawingArea.set_visible(true);
				m_noRomLabel.set_visible(false);
			}
		});
	});

	m_resetAction = Gio::SimpleAction::create("reset");
	m_resetAction->signal_activate().connect([this](const Glib::VariantBase &) {
		m_gameBoy->reset();

		muteAudio();
		resetAudio();
	});
	actionGroup->add_action(m_resetAction);
	m_resetAction->set_enabled(false);

	m_romCloseAction = Gio::SimpleAction::create("closeRom");
	m_romCloseAction->signal_activate().connect([this](const Glib::VariantBase &) {
		removeGameBoy();
		delete m_gameBoy;
		m_gameBoy = nullptr;

		set_title("Zirc Emulator");
		m_resetAction->set_enabled(false);
		m_romCloseAction->set_enabled(false);

		muteAudio();
		resetAudio();

		m_drawingArea.set_visible(false);
		m_noRomLabel.set_visible(true);
	});
	actionGroup->add_action(m_romCloseAction);
	m_romCloseAction->set_enabled(false);

	insert_action_group("app", actionGroup);

	auto menuBar = Gtk::make_managed<Gtk::PopoverMenuBar>(menubar);


	Gtk::Box box;
	box.set_orientation(Gtk::Orientation::VERTICAL);
	box.append(*menuBar);
	box.append(m_drawingArea);
	box.append(m_noRomLabel);

	m_drawingArea.set_visible(false);

	set_child(box);

	Glib::signal_timeout().connect(sigc::mem_fun(*this, &PlatformGtk::tick), 16);
}

const char *PlatformGtk::setupAudio() {
	PaError err = Pa_Initialize();
	if(err != paNoError) {
		const char *errorMsg = Pa_GetErrorText(err);
		std::cerr << "Platform[GTK]: Pa_Initialize() error: " << errorMsg << std::endl;
		return errorMsg;
	}

	err = Pa_OpenDefaultStream(&m_stream, 0, 1, paFloat32, AUDIO_SAMPLE_RATE, AUDIO_SAMPLE_AMOUNT, audioCallback, this);
	if(err != paNoError) {
		m_stream = nullptr;
		Pa_Terminate();
		const char *errorMsg = Pa_GetErrorText(err);
		std::cerr << "Platform[GTK]: Pa_OpenDefaultStream() error: " << errorMsg << std::endl;
		return errorMsg;
	}

	err = Pa_StartStream(m_stream);
	if(err != paNoError) {
		Pa_CloseStream(m_stream);
		m_stream = nullptr;
		Pa_Terminate();
		const char *errorMsg = Pa_GetErrorText(err);
		std::cerr << "Platform[GTK]: Pa_StartStream() error: " << errorMsg << std::endl;
		return errorMsg;
	}

	return nullptr;
}

void PlatformGtk::setupDrawingArea() {
	m_drawingArea.set_vexpand(true);
	m_drawingArea.set_hexpand(true);
	m_drawingArea.set_size_request(Lcd::WIDTH, Lcd::HEIGHT);

	m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::RGB24, Lcd::WIDTH, Lcd::HEIGHT);
	std::memset(m_backBuffer, 0, m_surface->get_stride() * Lcd::HEIGHT);

	m_drawingArea.set_draw_func(sigc::mem_fun(*this, &PlatformGtk::frameDrawCallback));
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
