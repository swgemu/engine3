#include "File.h"

File::File(const String& pathname) {
	name = pathname;

	fileDescriptor = NULL;

	mode = TEXT_MODE;
	access = READONLY_ACCESS;
}

bool File::open(int access) {
	if (fileDescriptor == NULL)
		fileDescriptor = fopen(name.toCharArray(), getModeString(mode, access));
	else
		fileDescriptor = freopen(name.toCharArray(), getModeString(mode, access), fileDescriptor);

	File::mode = mode;

	return fileDescriptor != NULL;
}

bool File::close() {
	return fclose(fileDescriptor) == 0;
}

bool File::deleteFile() {
	return remove(name.toCharArray()) == 0;
}

void File::flush() {
	fflush(fileDescriptor);
}

FILE* File::getDescriptor() const {
	return fileDescriptor;
}

const char* File::getModeString(int mode, int access) {
	String str;

	switch (access) {
	case READONLY_ACCESS:
		str = "r";
		break;
	case WRITEABLE_ACCESS:
		str = "w";
		break;
	default:
		return "";
	}

	if (mode == BINARY_MODE)
		str += "b";

	return str.toCharArray();
}

int File::seek(uint32 offset, int origin) {
	return fseek(fileDescriptor, offset, origin);
}

bool File::setReadOnly() {
	return open(File::READONLY_ACCESS);
}

bool File::setWriteable() {
	return open(File::WRITEABLE_ACCESS);
}
