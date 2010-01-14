/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef WEAKREFERENCE_H_
#define WEAKREFERENCE_H_

namespace sys {
  namespace lang {

	template<class O> class WeakReference : public Variable {
	protected:
		O object;

	public:
		WeakReference() : Variable() {
			object = NULL;
		}

		WeakReference(const WeakReference& ref) : Variable() {
			initializeObject(ref.object);
		}

		WeakReference(O obj) : Variable() {
			initializeObject(obj);
		}

		virtual ~WeakReference() {
			releaseObject();
		}

		virtual int compareTo(const WeakReference& val) const {
			if (object < val.object)
				return 1;
			else if (object > val.object)
				return -1;
			else
				return 0;
		}

		WeakReference& operator=(const WeakReference& ref) {
			if (this == &ref)
				return *this;

			updateObject(ref.object);

			return *this;
		}

		void operator=(O obj) {
			updateObject(obj);
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
				object->acquireWeak(this);
			}
		}

		inline void releaseObject() {
			if (object != NULL) {
				object->releaseWeak(this);
				object = NULL;
			}
		}
	};

  } // namespace lang
} // namespace sys

using namespace sys::lang;

#endif /* WEAKREFERENCE_H_ */
