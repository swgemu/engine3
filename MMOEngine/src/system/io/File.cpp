#include <sys/stat.h>

#include <errno.h>

#include "File.h"

File::File(const String& pathname) {
	name = pathname;

	fileDescriptor = NULL;

	mode = TEXT_MODE;
	access = READONLY_ACCESS;
	permissions = 0444;
}

bool File::open(int access) {
	String modestr = getModeString(mode, access);

	if (fileDescriptor == NULL)
		fileDescriptor = fopen(name.toCharArray(), modestr.toCharArray());
	else
		fileDescriptor = freopen(name.toCharArray(), modestr.toCharArray(), fileDescriptor);

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

String File::getModeString(int mode, int access) {
	String str;

	switch (access) {
	case READONLY_ACCESS:
		str = "r";
		break;
	case WRITEABLE_ACCESS:
		str = "w";
		break;
	case APPENDABLE_ACCESS:
		str = "a";
		break;
	default:
		return "";
	}

	if (mode == BINARY_MODE)
		str += "b";

	return str;
}

bool File::mkdir() {
    return doMkdir(name.toCharArray(), permissions);
}

bool File::doMkdir(const char* path, int mode) {
    struct stat st;
    int status = 0;

    if (stat(path, &st) != 0) {
        /* Directory does not exist */
        if (::mkdir(path, (mode_t) (mode | 0711)) != 0)
            status = -1;
    } else if (!S_ISDIR(st.st_mode)) {
        errno = ENOTDIR;
        status = -1;
    }

    return status != -1;
}

bool File::mkdirs() {
    char* pp;
    char* sp;
    int status;

    const char* path = name.toCharArray();
    char* copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0) {
        if (sp != pp) {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = doMkdir(copypath, permissions);
            *sp = '/';
        }

        pp = sp + 1;
    }

    if (status == 0)
        status = doMkdir(path, permissions);

    free(copypath);
    return (status);
}

int File::seek(uint32 offset, int origin) {
	return fseek(fileDescriptor, offset, origin);
}

int File::size() {
	int currentOffset = ftell(fileDescriptor);

	fseek(fileDescriptor, 0L, SEEK_END);

	int size = ftell(fileDescriptor);

	fseek(fileDescriptor, currentOffset, SEEK_SET);

	return size;
}

bool File::setReadOnly() {
	permissions = 0444;

	return open(File::READONLY_ACCESS);
}

bool File::setWriteable() {
	permissions = 0666;

	return open(File::WRITEABLE_ACCESS);
}

bool File::setAppendable() {
	permissions = 0666;

	return open(File::APPENDABLE_ACCESS);
}
