#include "File.h"

File::File(const String& pathname) {
	name = pathname;

	fileDescriptor = NULL;

	mode = READONLY;
}

bool File::open(int mode) {
	if (fileDescriptor == NULL)
		fileDescriptor = fopen(name.toCharArray(), getModeString(mode));
	else
		fileDescriptor = freopen(name.toCharArray(), getModeString(mode), fileDescriptor);

	File::mode = mode;

	return fileDescriptor != NULL;
}

bool File::close() {
	return fclose(fileDescriptor) == 0;
}

void File::flush() {
	fflush(fileDescriptor);
}

FILE* File::getDescriptor() const {
	return fileDescriptor;
}

char* File::getModeString(int mode) {
	switch (mode) {
	case READONLY:
		return "r";
	case WRITEABLE:
		return "w";
	case READONLY & WRITEABLE:
		return "rw";
	default:
		return "";
	}
}

int File::seek(uint32 offset, int origin) {
	return fseek(fileDescriptor, offset, origin);
}

bool File::setReadOnly() {
	return open(File::READONLY);
}

bool File::setWriteable() {
	return open(File::WRITEABLE);
}
