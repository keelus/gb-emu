#include <portaudio.h>

#include "gtk.hpp"

void PlatformGtk::setupWindow() {
	set_title("Zirc Emulator");
	set_default_size(512, 512);

	setupDrawingArea();
	setupKeyController();

	Gtk::Box box;
	set_child(box);
	box.set_orientation(Gtk::Orientation::VERTICAL);

	box.append(*m_menuBar.getMenuBar());

	box.append(m_drawingArea);
	m_drawingArea.set_visible(false);

	box.append(m_noRomLabel);
	m_noRomLabel.set_hexpand();
	m_noRomLabel.set_vexpand();

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
	m_drawingArea.set_vexpand();
	m_drawingArea.set_hexpand();
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
