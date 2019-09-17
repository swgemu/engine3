/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef ENGINE3_UNIQUEREFERENCE_H
#define ENGINE3_UNIQUEREFERENCE_H

#include <utility>

namespace sys {
	namespace lang {
		template<typename T>
		class UniqueReference {
			T obj{};

		public:
			UniqueReference() {
			}

			UniqueReference(const UniqueReference& ref) = delete;
			UniqueReference& operator=(const UniqueReference&) = delete;

			explicit UniqueReference(T object) : obj(object) {
			}

			UniqueReference(UniqueReference&& ref) : obj(std::move(ref.obj)) {
				ref.obj = nullptr;
			}

			UniqueReference& operator=(UniqueReference&& r) {
				if (obj == r.obj)
					return *this;

				if (this == &r)
					return *this;

				if (obj != nullptr) {
					delete obj;
			       	}

				obj = std::move(r.obj);

				r.obj = nullptr;

				return *this;
			}

			~UniqueReference() {
				if (obj != nullptr) {
					delete obj;
					obj = nullptr;
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

			explicit operator bool() const {
				return obj != nullptr;
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

		template<class T, class... Args>
		UniqueReference<T*> makeUnique(Args&&... args ) {
			return UniqueReference<T*>(new T(std::forward<Args>(args)...));
		}

		static_assert(sizeof(UniqueReference<int*>) == sizeof(int*), "UniqueReference sizeof check failed");

	}
}

using namespace sys::lang;

#endif //ENGINE3_UNIQUEREFERENCE_H
