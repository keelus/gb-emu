#include <gdkmm/glcontext.h>
#include <portaudio.h>

#include "gtk.hpp"

void PlatformGtk::setupWindow() {
	set_title("Zirc Emulator");
	set_default_size(512, 512);

	setupOpenGl();
	setupKeyController();

	Gtk::Box box;
	set_child(box);
	box.set_orientation(Gtk::Orientation::VERTICAL);

	box.append(*m_menuBar.getMenuBar());

	box.append(m_glArea);
	m_glArea.set_hexpand();
	m_glArea.set_vexpand();
	m_glArea.set_visible(false);

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

void PlatformGtk::setupOpenGl() {
	m_glArea.set_size_request(Lcd::WIDTH, Lcd::HEIGHT);
	m_glArea.set_required_version(3, 3);
	m_glArea.signal_render().connect(sigc::mem_fun(*this, &PlatformGtk::glDraw), false);
	m_glArea.signal_realize().connect(sigc::mem_fun(*this, &PlatformGtk::realizeOpenGl));
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

void PlatformGtk::realizeOpenGl() {
	m_glArea.make_current();
	m_glArea.throw_if_error();

	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "Platform[GTK]: GLEW init failed: " << glewGetErrorString(err) << std::endl;
		return;
	}

	const char *vertexShaderSource = "#version 330 core\n"
									 "layout (location = 0) in vec3 aPos;\n"
									 "layout (location = 1) in vec3 aColor;\n"
									 "layout (location = 2) in vec2 aTexCoord;\n"
									 "\n"
									 "out vec3 ourColor;\n"
									 "out vec2 TexCoord;\n"
									 "\n"
									 "void main()\n"
									 "{\n"
									 "    gl_Position = vec4(aPos, 1.0);\n"
									 "    ourColor = aColor;\n"
									 "    TexCoord = aTexCoord;\n"
									 "}\n";
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);

	const char *fragmentShaderSource = "#version 330 core\n"
									   "out vec4 FragColor;\n"
									   "\n"
									   "in vec3 ourColor;\n"
									   "in vec2 TexCoord;\n"
									   "\n"
									   "uniform sampler2D emulatorTexture;\n"
									   "\n"
									   "void main()\n"
									   "{\n"
									   "FragColor = texture(emulatorTexture, TexCoord);\n"
									   "}\n";
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);


	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, vertexShader);
	glAttachShader(m_shaderProgram, fragmentShader);
	glLinkProgram(m_shaderProgram);

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glUseProgram(m_shaderProgram);
	glUniform1i(glGetUniformLocation(m_shaderProgram, "emulatorTexture"), 0);

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &m_VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);


	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_STATIC_DRAW);


	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
	glEnableVertexAttribArray(0);


	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Lcd::WIDTH, Lcd::HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, m_frontBuffer);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
