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

#include <cassert>
#include <list>
#include <memory>
#include <mutex>

#include "Fiv.hpp"
#include "Image.hpp"

using namespace std;

Fiv::Images::Images(shared_ptr<Fiv> fiv_) : fiv(fiv_) {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	assert(fiv->images.size());
	it = fiv->images.cbegin();
}

shared_ptr<Image> Fiv::Images::current() {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	return *it;
}

void Fiv::Images::orientation(Image::Orientation modify) {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	auto image = *it;
	image->setOrientation(modify);
	if (image->loadThumbnail())
		image->getThumbnail()->setOrientation(modify);
}

bool Fiv::Images::first() {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	if (it != fiv->images.cbegin()) {
		it = fiv->images.cbegin();
		return true;
	}
	return false;
}

bool Fiv::Images::previous() {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	if (it != fiv->images.cbegin()) {
		it--;
		return true;
	}
	return false;
}

bool Fiv::Images::next() {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	auto tmp = it;
	tmp++;
	if (tmp != fiv->images.cend()) {
		it++;
		return true;
	}
	return false;
}

bool Fiv::Images::last() {
	unique_lock<mutex> lckImages(fiv->mtxImages);
	if (it != fiv->images.cend()) {
		it = fiv->images.cend();
		return true;
	}
	return false;
}
