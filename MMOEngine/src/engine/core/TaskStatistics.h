#ifndef TASKSTATISTICS_H_
#define TASKSTATISTICS_H_

#include "system/io/ObjectOutputStream.h"
#include "system/io/ObjectInputStream.h"

namespace engine {
  namespace core {

      class TaskStatistics {
      public:
          uint64 totalRunTime;
          uint64 maxRunTime;
          uint64 totalRunCount;
          uint64 minRunTime;

          int compareTo(const TaskStatistics& t) const {
            if (totalRunTime < t.totalRunTime)
              return 1;
            else if (totalRunTime > t.totalRunTime)
              return -1;
            else {
              if (maxRunTime < t.maxRunTime)
                return 1;
              else if (maxRunTime > t.maxRunTime)
                return -1;
              else {
                if (totalRunCount < t.totalRunCount)
                  return 1;
                else if (totalRunCount > t.totalRunCount)
                  return -1;
                else
                  return 0;
              }
            }
          }

          bool toBinaryStream(ObjectOutputStream* stream) {
            return true;
          }

          bool parseFromBinaryStream(ObjectInputStream* stream) {
            return true;
          }
      };

  } // namespace core
} // namespace engine

using namespace engine::core;

#endif /* TASKSTATISTICS_H_ */
