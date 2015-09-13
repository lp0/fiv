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

#include "Codec.hpp"

#include <memory>

#include "Image.hpp"

using namespace std;

Codec::Codec() {

}

Codec::~Codec() {

}

Codec::Codec(shared_ptr<const Image> image_) : image(image_) {

}

unique_ptr<Codec> Codec::getInstance(shared_ptr<const Image> image_ __attribute__((unused))) const {
	return unique_ptr<Codec>();
}

int Codec::getWidth() {
	return 0;
}

int Codec::getHeight() {
	return 0;
}

Image::Orientation Codec::getOrientation() {
	return Image::Orientation(Image::Rotate::ROTATE_NONE, false);
}

Cairo::RefPtr<Cairo::Surface> Codec::getPrimary() {
	return Cairo::RefPtr<Cairo::Surface>();
}

shared_ptr<Image> Codec::getThumbnail() {
	return shared_ptr<Image>();
}
