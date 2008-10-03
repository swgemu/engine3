#ifndef WRITER_H_
#define WRITER_H_

namespace sys {
  namespace io {
  
  	class Writer {
  	public:
  		virtual ~Writer() {
  		}
  		
  		virtual void close() = 0;

  		virtual void flush() = 0;

  		virtual int write(byte* buf, int len) = 0;

  		virtual int write(byte* buf, uint32 off, int len) = 0;
  	};

  } // namespace io
} // namespace sys

using namespace sys::io;

#endif /*WRITER_H_*/
