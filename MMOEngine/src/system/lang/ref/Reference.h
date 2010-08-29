/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef REFERENCE_H_
#define REFERENCE_H_

#include "../Object.h"

namespace sys {
  namespace lang {

	template<class O> class Reference : public Variable {
	protected:
		O object;

	public:
		Reference() : Variable() {
			object = NULL;
		}

		Reference(const Reference& ref) : Variable() {
			initializeObject(ref.object);
		}

		Reference(O obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~Reference() {
			releaseObject();
		}

		virtual int compareTo(const Reference& val) const {
			if (object < val.object)
				return 1;
			else if (object > val.object)
				return -1;
			else
				return 0;
		}

		Reference& operator=(const Reference& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.object);

			return *this;
		}

		O operator=(O obj) {
			updateObject(obj);

			return object;
		}

		O operator->() const {
			return object;
		}

		operator O() const {
			return object;
		}

		inline O get() const {
			return object;
		}

		bool toString(String& str) {
			return object->toString(str);
		}

		bool parseFromString(const String& str, int version = 0) {
			return object->parseFromString(str, version);
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return object->toBinaryStream(stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return object->parseFromBinaryStream(stream);
		}

	protected:
		inline void updateObject(O obj) {
			if (obj == object)
				return;

			releaseObject();

			setObject(obj);
		}

		inline void setObject(O obj) {
			if (obj == object)
				return;

			initializeObject(obj);
		}

		inline void initializeObject(O obj) {
			object = obj;

			acquireObject();
		}

		inline void acquireObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCES
				object->addHolder(this);
			#endif
				object->acquire();
			}
		}

		inline void releaseObject() {
			if (object != NULL) {
			#ifdef TRACE_REFERENCES
				object->removeHolder(this);
			#endif
				object->release();
				object = NULL;
			}
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /*REFERENCE_H_*/
