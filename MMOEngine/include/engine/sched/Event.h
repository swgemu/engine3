/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef EVENT_H_
#define EVENT_H_

#include <signal.h>

namespace engine {
  namespace sched {

	class EventQueue;

	class Event : public AdvancedBinaryHeapNode {
	protected:
		Time timestamp;

		bool enQueued;
		bool keepEvent;

	public:
		Event() : AdvancedBinaryHeapNode() {
			enQueued = false;
			keepEvent = false;
		}

		Event(uint64 mtime) : AdvancedBinaryHeapNode() {
			timestamp.addMiliTime(mtime);

			enQueued = false;
			keepEvent = false;
		}

		Event(Time& time) : AdvancedBinaryHeapNode() {
			timestamp = time;

			enQueued = false;
			keepEvent = false;
		}

		virtual ~Event() {
			if (enQueued) {
				System::out << "ERROR: scheduled event deleted\n";
				raise(SIGSEGV);
			}
		}

		int compareTo(AdvancedBinaryHeapNode* node) {
			Event* e = (Event*) node;

			int cmp = timestamp.compareTo(e->timestamp);
			if (cmp == 0) {
				if (this == e)
					return 0;
				else if (this < e)
					return 1;
				else
					return -1;
			} else
				return cmp;
		}

		virtual bool activate() = 0;

	protected:
		void set(Time& time) {
			timestamp = time;
		}

		void update(uint64 mtime = 0) {
			timestamp.update();

			if (mtime != 0)
				timestamp.addMiliTime(mtime);
		}

	public:
		inline void setQueued(bool state) {
			enQueued = state;
		}

		inline void setKeeping(bool keep) {
			keepEvent = keep;
		}

		inline Time& getTimeStamp() {
			return timestamp;
		}

		inline bool isQueued() {
			return enQueued;
		}

		inline bool doKeep() {
			return keepEvent;
		}

		String toString() {
			struct timespec* ts = timestamp.getTimeSpec();

			StringBuffer s;
			s << timestamp.getMiliTime() << "[" << ts->tv_sec << "/" << ts->tv_nsec << "] (ptr = " << this << ")";
			return s.toString();
		}

		friend class EventQueue;
	};

  } // namespace sched
} // namespace engine

using namespace engine::sched;

#endif /*EVENT_H_*/
