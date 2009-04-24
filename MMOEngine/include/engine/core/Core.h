#ifndef CORE_H_
#define CORE_H_

#include "system/lang.h"

#include "../log/Logger.h"

#include "../db/MySqlDatabase.h"

#include <new>

namespace engine {
  namespace core {

	class Core {
	public:
		Core() {
			std::set_new_handler(outOfMemoryHandler);

			Socket::initialize();
		}

		Core(const char* globallogfile) {
			std::set_new_handler(outOfMemoryHandler);

			Socket::initialize();

			Logger::setGlobalFileLogger(globallogfile);
		}

		virtual ~Core() {
			MySqlDatabase::finalizeLibrary();

			NetworkInterface::finalize();

			Logger::closeGlobalFileLogger();
		}

		static void outOfMemoryHandler() {
			System::out << "OutOfMemoryException\n";

			//StackTrace::printStackTrace();

			exit(1);
		}

	};

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /*CORE_H_*/
