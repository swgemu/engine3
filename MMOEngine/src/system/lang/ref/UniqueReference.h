/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef ENGINE3_UNIQUEREFERENCE_H
#define ENGINE3_UNIQUEREFERENCE_H

namespace sys {
	namespace lang {
		template<typename T>
		class UniqueReference {
			T obj;

		public:
			UniqueReference() : obj(nullptr) {

			}

			UniqueReference(T object) : obj(object) {

			}

			~UniqueReference() {
				if (obj) {
					delete obj;
				}
			}

			UniqueReference& operator=(T object) {
				if (obj == object)
					return *this;

				if (obj) {
					delete obj;
				}

				obj = object;

				return *this;
			}

			bool operator==(T object) const {
				return object == obj;
			}

			T operator->() const {
				return obj;
			}

			T get() const {
				return obj;
			}

			operator T() const {
				return obj;
			}
		};

	}
}

using namespace sys::lang;

#endif //ENGINE3_UNIQUEREFERENCE_H
