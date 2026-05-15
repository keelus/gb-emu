#pragma once

#include <cstring>
#include <gtkmm/drawingarea.h>

#include "backends/video/video_backend.hpp"
#include <core/graphics/lcd.hpp>

class VideoBackendSoftware : public VideoBackend {
  public:
	void initialize() override {
		m_drawingArea.set_vexpand();
		m_drawingArea.set_hexpand();
		m_drawingArea.set_size_request(Lcd::WIDTH, Lcd::HEIGHT);

		m_surface = Cairo::ImageSurface::create(Cairo::Surface::Format::RGB24, Lcd::WIDTH, Lcd::HEIGHT);
		std::memset(m_backBuffer, 0, m_surface->get_stride() * Lcd::HEIGHT);

		m_drawingArea.set_draw_func(sigc::mem_fun(*this, &VideoBackendSoftware::render));
	}

	void queueRender() override { m_drawingArea.queue_draw(); }

	void swapBuffers() override {
		std::memcpy(m_surface->get_data(), m_backBuffer, sizeof(unsigned char) * m_surface->get_stride() * Lcd::HEIGHT);
		std::memset(m_backBuffer, 0, sizeof(unsigned char) * m_surface->get_stride() * Lcd::HEIGHT);
	}

	void setVisible(bool visible) override { m_drawingArea.set_visible(visible); }
	Gtk::DrawingArea *getGtkWidget() override { return &m_drawingArea; }

	void drawPixel(uint8_t x, uint8_t y, Color color) override {
		char *px = m_backBuffer + y * m_surface->get_stride() + x * 4;
		px[0] = color.blue();
		px[1] = color.green();
		px[2] = color.red();
	}

  private:
	void render(const Cairo::RefPtr<Cairo::Context> &cr, int w, int h) {
		m_surface->flush();

		unsigned char *pixels = m_surface->get_data();
		int stride = m_surface->get_stride();

		m_surface->mark_dirty();

		cr->set_source_rgb(0, 0, 0);
		cr->paint();

		double size = std::min(w, h);
		double offsetX = (w - size) / 2.0;
		double offsetY = (h - size) / 2.0;

		cr->translate(offsetX, offsetY);
		cr->scale(size / Lcd::WIDTH, size / Lcd::HEIGHT);

		auto pattern = Cairo::SurfacePattern::create(m_surface);
		pattern->set_filter(Cairo::SurfacePattern::Filter::NEAREST);
		cr->set_source(pattern);
		cr->paint();
	}

	Gtk::DrawingArea m_drawingArea;
	Cairo::RefPtr<Cairo::ImageSurface> m_surface;
	char m_backBuffer[Lcd::WIDTH * Lcd::HEIGHT * 4];
};
