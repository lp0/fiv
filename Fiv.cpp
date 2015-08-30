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

#include "Fiv.hpp"

#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <algorithm>
#include <condition_variable>
#include <cstdio>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>

#include "FileDataBuffer.hpp"
#include "Image.hpp"
#include "JpegCodec.hpp"

using namespace std;

std::map<std::string,std::shared_ptr<Codec>> Fiv::codecs;

int Fiv::init(int argc, char *argv[]) {
	int ret;

	initCodecs();

	ret = initImages(argc, argv);
	if (ret != EXIT_SUCCESS)
		return ret;

	return EXIT_SUCCESS;
}

unique_ptr<Codec> Fiv::getCodec(shared_ptr<const Image> image, string mimeType) {
	try {
		return codecs.at(mimeType)->getInstance(image);
	} catch (const out_of_range &oor) {
		return unique_ptr<Codec>();
	}
}

void Fiv::initCodecs() {
	codecs[JpegCodec::MIME_TYPE] = make_shared<JpegCodec>();
}

int Fiv::initImages(int argc, char *argv[]) {
	unique_ptr<list<string>> args(make_unique<list<string>>());

	while (--argc > 0)
		args->emplace_back((const char *)(++argv)[0]);

	if (!args->size())
		args->emplace_back(".");

	return initImagesInBackground(move(args));
}

int Fiv::initImagesInBackground(unique_ptr<list<string>> filenames_) {
	auto self(shared_from_this());

	thread([this, self, filenames = move(filenames_)] () mutable {
		this->initImagesThread(move(filenames));
	}).detach();

	unique_lock<mutex> lckImages(mtxImages);
	if (!initImagesComplete)
		imageAdded.wait(lckImages);

	for (auto image : images) {
		cout << *image << endl;
		cout << *image->getThumbnail() << endl;
	}

	return images.size() ? EXIT_SUCCESS : EXIT_FAILURE;
}

void Fiv::initImagesThread(unique_ptr<list<string>> filenames) {
	for (auto filename : *filenames) {
		struct stat st;

		if (access(filename.c_str(), R_OK)) {
			perror(filename.c_str());
			continue;
		}

		if (stat(filename.c_str(), &st))
			continue;

		if (S_ISREG(st.st_mode)) {
			unique_ptr<DataBuffer> buffer(make_unique<FileDataBuffer>(filename));
			shared_ptr<Image> image(make_shared<Image>(filename, move(buffer)));

			if (image->load()) {
				unique_lock<mutex> lckImages(mtxImages);

				images.push_back(image);
				imageAdded.notify_all();
			}
		} else if (S_ISDIR(st.st_mode)) {
			deque<shared_ptr<Image>> dirImages;

			initImagesFromDir(filename, dirImages);

			for (auto image : dirImages) {
				if (image->load()) {
					unique_lock<mutex> lckImages(mtxImages);

					images.push_back(image);
					imageAdded.notify_all();
				}
			}
		}
	}

	unique_lock<mutex> lckImages(mtxImages);
	initImagesComplete = true;
	imageAdded.notify_all();
}

static bool compareImage(const shared_ptr<Image> &a, const shared_ptr<Image> &b) {
	return a->name < b->name;
}

void Fiv::initImagesFromDir(const string &dirname, deque<shared_ptr<Image>> &dirImages) {
	DIR *dir = opendir(dirname.c_str());
	struct dirent *entry;

	if (dir == nullptr) {
		perror(dirname.c_str());
		return;
	}

	while ((entry = readdir(dir)) != NULL) {
		if (entry->d_type == DT_REG || entry->d_type == DT_LNK) {
			string filename = dirname + "/" + entry->d_name;

			if (entry->d_type == DT_LNK) {
				struct stat st;

				if (stat(filename.c_str(), &st))
					continue;

				if (!S_ISREG(st.st_mode)) // links must be to regular files
					continue;
			}

			if (access(filename.c_str(), R_OK)) {
				perror(filename.c_str());
				continue;
			}

			unique_ptr<DataBuffer> buffer(make_unique<FileDataBuffer>(filename));
			dirImages.emplace_back(make_shared<Image>(filename, move(buffer)));
		}
	}

	closedir(dir);

	sort(dirImages.begin(), dirImages.end(), compareImage);
}
