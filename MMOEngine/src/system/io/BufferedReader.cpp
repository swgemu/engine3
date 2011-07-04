#include "BufferedReader.h"

BufferedReader::BufferedReader(Reader* reader) {
	BufferedReader::reader = reader;
}

void BufferedReader::close() {
	reader->close();
}

int BufferedReader::read(char* buf, int len) {
	return reader->read(buf, len);
}

int BufferedReader::read(char* buf, uint32 off, int len) {
	return reader->read(buf, off, len);
}

int BufferedReader::skip(int n) {
	return reader->skip(n);
}
