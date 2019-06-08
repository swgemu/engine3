/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef RUNSTATISTICS_H_
#define RUNSTATISTICS_H_

namespace sys {
    namespace io {
        class ObjectOutputStream;
        class ObjectInputStream;
    }
}

namespace engine {
  namespace core {

      class RunStatistics {
      public:
          uint64 totalRunTime;
          uint64 maxRunTime;
          uint64 totalRunCount;
          uint64 minRunTime;

          int compareTo(const RunStatistics& t) const {
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

      class RunStatisticsOrderedByMaxTime {
      public:
          uint64 totalRunTime;
          uint64 maxRunTime;
          uint64 totalRunCount;
          uint64 minRunTime;

          RunStatisticsOrderedByMaxTime() = default;
          RunStatisticsOrderedByMaxTime(const RunStatisticsOrderedByMaxTime&) = default;

          RunStatisticsOrderedByMaxTime(const RunStatistics& r) : totalRunTime(r.totalRunTime), maxRunTime(r.maxRunTime),
                                                                    totalRunCount(r.totalRunCount), minRunTime(r.minRunTime) {
          }

          int compareTo(const RunStatisticsOrderedByMaxTime& t) const {
              if (maxRunTime < t.maxRunTime)
                  return 1;
              else if (maxRunTime > t.maxRunTime)
                  return -1;
              else {
                  if (totalRunTime < t.totalRunTime)
                      return 1;
                  else if (totalRunTime > t.totalRunTime)
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

#endif /* RUNSTATISTICS_H_ */
