#pragma once

#include <gtkmm/alertdialog.h>
#include <string>

static void showErrorDialog(const std::string &title, const std::string &content) {
	auto dialog = Gtk::AlertDialog::create();
	dialog->set_message(title);
	dialog->set_detail(content);
	dialog->set_modal(true);
	dialog->show();
}
