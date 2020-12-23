/*
 Copyright 2015,2020  Simon Arlott

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

#include "FileDataBuffer.hpp"

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstdint>
#include <cstdio>
#include <iostream>
#include <string>

#include "ThreadLocalStream.hpp"

using namespace std;

FileDataBuffer::FileDataBuffer(const string &filename_) : filename(filename_) {

}

FileDataBuffer::~FileDataBuffer() {
	unload();
}

bool FileDataBuffer::load() {
	int fd = -1;

	if (mapping == nullptr) {
		struct stat st;

		fd = open(filename.c_str(), O_RDONLY|O_NONBLOCK|O_CLOEXEC);
		if (fd < 0)
			goto err;

		if (fstat(fd, &st))
			goto err;

		mapping = mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
		if (mapping == nullptr)
			goto err;

		madvise(mapping, st.st_size, MADV_DONTDUMP);

		data = static_cast<uint8_t*>(mapping);
		length = st.st_size;
		close(fd);
	}

	return true;

err:
	ThreadLocalEStream::perror(filename);
	if (fd >= 0)
		close(fd);
	return false;
}

void FileDataBuffer::unload() {
	if (mapping != nullptr) {
		munmap(mapping, length);

		mapping = nullptr;
	}
}

string FileDataBuffer::getFilename() {
	return filename;
}
