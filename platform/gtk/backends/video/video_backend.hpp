#pragma once

#include <cstdint>
#include <gtkmm/widget.h>
#include <zirc/common.hpp>

class VideoBackend {
  public:
	virtual void initialize() = 0;

	virtual void queueRender() = 0;
	virtual void swapBuffers() = 0;

	virtual void setVisible(bool visible) = 0;
	virtual Gtk::Widget *getGtkWidget() = 0;

	virtual void drawPixel(uint8_t x, uint8_t y, Zirc::Color color) = 0;
};
