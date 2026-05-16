#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <cstring>
#include <gtkmm/glarea.h>
#include <iostream>
#include <zirc/common.hpp>
#include <zirc/graphics/lcd.hpp>

#include "backends/video/video_backend.hpp"
#include "utils.hpp"

class VideoBackendOpenGl : public VideoBackend {
  public:
	void initialize() override {
		m_glArea.set_size_request(Zirc::Lcd::WIDTH, Zirc::Lcd::HEIGHT);
		m_glArea.set_required_version(3, 3);
		m_glArea.signal_render().connect(sigc::mem_fun(*this, &VideoBackendOpenGl::render), false);
		m_glArea.signal_realize().connect(sigc::mem_fun(*this, &VideoBackendOpenGl::realize));
		m_glArea.signal_resize().connect(sigc::mem_fun(*this, &VideoBackendOpenGl::resize));
		m_glArea.set_hexpand();
		m_glArea.set_vexpand();
	}

	~VideoBackendOpenGl() {
		if(!m_initialized) { return; }

		if(m_texture) {
			glDeleteTextures(1, &m_texture);
			m_texture = 0;
		}

		if(m_VAO) {
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}

		if(m_VBO) {
			glDeleteBuffers(1, &m_VBO);
			m_VBO = 0;
		}

		if(m_EBO) {
			glDeleteBuffers(1, &m_EBO);
			m_EBO = 0;
		}

		if(m_shaderProgram) {
			glDeleteShader(m_shaderProgram);
			m_shaderProgram = 0;
		}
	}

	void queueRender() override { m_glArea.queue_render(); }

	void swapBuffers() override {
		std::memcpy(m_frontBuffer, m_backBuffer, sizeof(m_backBuffer));
		std::memset(m_backBuffer, 0, sizeof(m_backBuffer));
	}

	void setVisible(bool visible) override { m_glArea.set_visible(visible); }

	Gtk::GLArea *getGtkWidget() override { return &m_glArea; }

	void drawPixel(uint8_t x, uint8_t y, Zirc::Color color) override {
		GLubyte *px = m_backBuffer + (Zirc::Lcd::HEIGHT - 1 - y) * Zirc::Lcd::WIDTH * 3 + x * 3;
		px[0] = color.red();
		px[1] = color.green();
		px[2] = color.blue();
	}

  private:
	void resize(int width, int height) {
		if(width < 0) { width = 1; }
		if(height < 0) { height = 1; }

		float viewportAspect = float(width) / float(height);
		float lcdAspect = float(Zirc::Lcd::WIDTH) / float(Zirc::Lcd::HEIGHT);

		float scaleX, scaleY;
		if(lcdAspect < viewportAspect) {
			scaleX = lcdAspect / viewportAspect;
			scaleY = 1.0f;
		} else {
			scaleX = 1.0f;
			scaleY = viewportAspect / lcdAspect;
		}

		glUseProgram(m_shaderProgram);
		GLint loc = glGetUniformLocation(m_shaderProgram, "u_aspect");
		glUniform2f(loc, scaleX, scaleY);
	}


	bool render(const Glib::RefPtr<Gdk::GLContext> &context) {
		if(!m_initialized) { return true; }

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(m_shaderProgram);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, Zirc::Lcd::WIDTH, Zirc::Lcd::HEIGHT, GL_RGB, GL_UNSIGNED_BYTE,
						m_frontBuffer);

		glBindVertexArray(m_VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		return true;
	}

	void realize() {
		m_glArea.make_current();
		try {
			m_glArea.throw_if_error();
		} catch(Gdk::GLError error) {
			std::cerr << "Platform[GTK]: OpenGLinit failed: " << error.what() << std::endl;
			showErrorDialog("OpenGL Error", "OpenGL could not be initialized: \"" + std::string(error.what()) + "\"");

			return;
		}

		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if(err != GLEW_OK) {
			const char *errorMsg = (const char *)glewGetErrorString(err);
			std::cerr << "Platform[GTK]: GLEW init failed: \"" << errorMsg << "\"" << std::endl;
			showErrorDialog("GLEW error", "GLEW could not be initialized: \"" + std::string(errorMsg) + "\"");

			return;
		}

		setupShaders();
		setupArrays();
		setupTexture();

		m_initialized = true;
	}

	void setupShaders() {
		const char *vertexShaderSource = "#version 330 core\n"
										 "layout (location = 0) in vec3 aPos;\n"
										 "layout (location = 1) in vec3 aColor;\n"
										 "layout (location = 2) in vec2 aTexCoord;\n"
										 "\n"
										 "uniform vec2 u_aspect;\n"
										 "\n"
										 "out vec3 ourColor;\n"
										 "out vec2 TexCoord;\n"
										 "\n"
										 "void main()\n"
										 "{\n"
										 "    vec3 pos = aPos;\n"
										 "    pos.x *= u_aspect.x;\n"
										 "    pos.y *= u_aspect.y;\n"
										 "    gl_Position = vec4(pos, 1.0);\n"
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
	}

	void setupArrays() {
		glGenVertexArrays(1, &m_VAO);
		glGenBuffers(1, &m_VBO);
		glGenBuffers(1, &m_EBO);

		glBindVertexArray(m_VAO);

		float vertices[32] = {
			-1.0f, 1.0f,  0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
			1.0f,  -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,	 1.0f,	0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		};

		glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		unsigned int indices[6] = {0, 1, 2, 0, 2, 3};

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	void setupTexture() {
		glGenTextures(1, &m_texture);
		glBindTexture(GL_TEXTURE_2D, m_texture);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, Zirc::Lcd::WIDTH, Zirc::Lcd::HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE,
					 m_frontBuffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	Gtk::GLArea m_glArea;

	GLuint m_VAO = 0, m_VBO = 0, m_EBO = 0;
	GLuint m_shaderProgram = 0, m_texture = 0;

	GLubyte m_backBuffer[Zirc::Lcd::WIDTH * Zirc::Lcd::HEIGHT * 3] = {0};
	GLubyte m_frontBuffer[Zirc::Lcd::WIDTH * Zirc::Lcd::HEIGHT * 3] = {0};

	bool m_initialized = false;
};
