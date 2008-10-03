#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "../db/MySqlDatabase.h"

namespace engine {
  namespace core {

	class Core {
	public:
		Core() {
			set_new_handler(outOfMemoryHandler);

			Socket::initialize();
		}

		Core(const char* globallogfile) {
			set_new_handler(outOfMemoryHandler);

			Socket::initialize();

			Logger::setGlobalFileLogger(globallogfile);
		}

		virtual ~Core() {
			MySqlDatabase::finalizeLibrary();

			NetworkInterface::finalize();
			
			Logger::closeGlobalFileLogger();
		}

		static void outOfMemoryHandler() {
			cout << "OutOfMemoryException\n";
			
			exit(1);
		}

	};
	
  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
