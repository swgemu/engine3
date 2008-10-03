#ifndef READER_H_
#define READER_H_

namespace sys {
  namespace io {
  
  	class Reader {
  	public:
  		virtual ~Reader() {
  		}
  		
  		virtual void close() = 0;
  		
  		virtual int read(byte* buf, int len) = 0;

  		virtual int read(byte* buf, uint32 off, int len) = 0;
  		
  		virtual int skip(int n) = 0;
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*READER_H_*/
