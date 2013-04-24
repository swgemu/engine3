#ifndef TASKSTATISTICS_H_
#define TASKSTATISTICS_H_

#include "Task.h"

#include "engine/util/Singleton.h"

namespace engine {
  namespace core {

	/*class TaskStatistics : public Logger, public Singleton<TaskStatistics> {
	public:
		TaskStatistics();

		void taskFinished(Task* task, uint64 executionTime);
	};*/

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKSTATISTICS_H_ */
