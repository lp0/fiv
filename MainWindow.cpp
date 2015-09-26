/*
 Copyright 2015  Simon Arlott

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "MainWindow.hpp"

#include <gdk/gdkevents.h>
#include <glibmm/refptr.h>
#include <glibmm/signalproxy.h>
#include <gtkmm/gesturedrag.h>
#include <gtkmm/gesturezoom.h>
#include <sigc++/connection.h>
#include <sigc++/functors/mem_fun.h>
#include <memory>
#include <string>
#include <utility>

#include "Fiv.hpp"
#include "Image.hpp"
#include "ImageDrawable.hpp"

using namespace std;

MainWindow::MainWindow(shared_ptr<Fiv> fiv) : Gtk::ApplicationWindow() {
	images = fiv;
	fullScreen = false;

	set_default_size(1920/2, 1080/2);

	drawImage.setImages(images);
	add(drawImage);

	add_action("edit.mark", sigc::mem_fun(this, &MainWindow::action_edit_mark));
	add_action("edit.toggleMark", sigc::mem_fun(this, &MainWindow::action_edit_toggleMark));
	add_action("edit.unmark", sigc::mem_fun(this, &MainWindow::action_edit_unmark));
	add_action("image.rotateLeft", sigc::mem_fun(this, &MainWindow::action_image_rotateLeft));
	add_action("image.rotateRight", sigc::mem_fun(this, &MainWindow::action_image_rotateRight));
	add_action("image.flipHorizontal", sigc::mem_fun(this, &MainWindow::action_image_flipHorizontal));
	add_action("image.flipVertical", sigc::mem_fun(this, &MainWindow::action_image_flipVertical));
	add_action("view.first", sigc::mem_fun(this, &MainWindow::action_view_first));
	add_action("view.previous", sigc::mem_fun(this, &MainWindow::action_view_previous));
	add_action("view.next", sigc::mem_fun(this, &MainWindow::action_view_next));
	add_action("view.last", sigc::mem_fun(this, &MainWindow::action_view_last));
	add_action("view.zoomActual", sigc::mem_fun(drawImage, &ImageDrawable::zoomActual));
	add_action("view.zoomFit", sigc::mem_fun(drawImage, &ImageDrawable::zoomFit));
	add_action("view.fullScreen", sigc::mem_fun(this, &MainWindow::action_view_fullScreen));
	add_action("view.afPoints", sigc::mem_fun(drawImage, &ImageDrawable::toggleAfPoints));

	drag = Gtk::GestureDrag::create(drawImage);
	drag->signal_drag_begin().connect(sigc::mem_fun(drawImage, &ImageDrawable::dragBegin));
	drag->signal_drag_update().connect(sigc::mem_fun(drawImage, &ImageDrawable::dragUpdate));
	drag->signal_drag_end().connect(sigc::mem_fun(drawImage, &ImageDrawable::dragEnd));

	zoom = Gtk::GestureZoom::create(drawImage);
	zoom->signal_scale_changed().connect(sigc::mem_fun(drawImage, &ImageDrawable::applyZoom));

	updateAll();
}

void MainWindow::addImage() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	updateTitle();
}

void MainWindow::loadedCurrent() {
	drawImage.loaded();
}

void MainWindow::action_edit_mark() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->mark(images->current()))
		updateTitle();
}

void MainWindow::action_edit_toggleMark() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->toggleMark(images->current()))
		updateTitle();
}

void MainWindow::action_edit_unmark() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->unmark(images->current()))
		updateTitle();
}

void MainWindow::action_image_rotateLeft() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	images->orientation(Image::Orientation(Image::Rotate::ROTATE_270, false));
	updateAll();
}

void MainWindow::action_image_rotateRight() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	images->orientation(Image::Orientation(Image::Rotate::ROTATE_90, false));
	updateAll();
}

void MainWindow::action_image_flipHorizontal() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	images->orientation(Image::Orientation(Image::Rotate::ROTATE_NONE, true));
	updateAll();
}

void MainWindow::action_image_flipVertical() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	images->orientation(Image::Orientation(Image::Rotate::ROTATE_180, true));
	updateAll();
}

void MainWindow::action_view_previous() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->previous())
		updateAll();
}

void MainWindow::action_view_next() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->next())
		updateAll();
}

void MainWindow::action_view_first() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->first())
		updateAll();
}

void MainWindow::action_view_last() {
	lock_guard<mutex> lckUpdate(mtxUpdate);
	if (images->last())
		updateAll();
}

void MainWindow::action_view_fullScreen() {
	if (fullScreen) {
		unfullscreen();
	} else {
		fullscreen();
	}
	redraw();
}

void MainWindow::redraw() {
	drawImage.redraw();
}

void MainWindow::updateAll() {
	drawImage.update();
	updateTitle();
}

void MainWindow::updateTitle() {
	auto image = images->current();
	pair<int,int> pos = images->position();
	stringstream title;

	title << Fiv::appName << ": " << image->name;
	if (images->hasMarkSupport())
		title << (images->isMarked(image) ? " \u2611" : " \u2610");
	title << " (" << pos.first << "/" << pos.second << ")";

	set_title(title.str());
}

bool MainWindow::on_window_state_event(GdkEventWindowState *event) {
	fullScreen = (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN) != 0;
	return false;
}
