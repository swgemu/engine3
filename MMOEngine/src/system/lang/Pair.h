/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : Pair
 * @created     : Friday Aug 09, 2019 07:38:44 UTC
 */

#ifndef E3PAIR_H
#define E3PAIR_H

#include <utility>
#include "types.h"

namespace sys {
  namespace lang {
	  template<class A, class B>
	  struct Pair {
	  public:
		typedef A first_type;
		typedef B second_type;

		A first;
		B second;

		Pair() : first(), second() {
		}

		Pair(const A& first, const B& second) : first(first),
							second(second) {
		}

		Pair(const Pair& p) : first(p.first), second(p.second) {
		}

		Pair(Pair&& pair) : first(std::move(pair.first)),
					second(std::move(pair.second)) {
		}

		template<class A2, class B2>
	       	Pair(A2&& a, B2&& b) : first(std::forward<A2>(a)),
					second(std::forward<B2>(b)) {
		}

		template<class A2, class B2>
	       	Pair(const Pair<A2, B2>& pair) : first(pair.first),
					second(pair.second) {
		}

		template<class A2, class _Arg0, class... _Args>
	        Pair(A2&& __x, _Arg0&& __arg0, _Args&&... __args)
		     : first(std::forward<A2>(__x)),
			second(std::forward<_Arg0>(__arg0),
			std::forward<_Args>(__args)...) {
		}

		Pair& operator=(const Pair& p) {
			first = p.first;
			second = p.second;

			return *this;
		}

		Pair& operator=(Pair&& p) {
			first = std::move(p.first);
			second = std::move(p.second);

			return *this;
		}

		template<class A2, class B2>
		Pair& operator=(Pair<A2, B2>&& p) {
			first = std::move(p.first);
			second = std::move(p.second);

			return *this;
		}

		void swap(Pair&& p) {
			std::swap(first, p.first);
			std::swap(second, p.second);
		}

		int compareTo(const Pair& pair2) {
			if (*this < pair2)
				return 1;
			else if (*this > pair2) {
				return -1;
			} else {
				return 0;
			}
		}

		bool toBinaryStream(ObjectOutputStream* stream) {
			return TypeInfo<A>::toBinaryStream(&first, stream) &&
				TypeInfo<B>::toBinaryStream(&second, stream);
		}

		bool parseFromBinaryStream(ObjectInputStream* stream) {
			return TypeInfo<A>::parseFromBinaryStream(&first, stream) &&
				TypeInfo<B>::parseFromBinaryStream(&second, stream);

		}
	};

	template<class _T1, class _T2>
	inline bool operator==(const Pair<_T1, _T2>& __x, const Pair<_T1, _T2>& __y) {
		return __x.first == __y.first && __x.second == __y.second;
	}

	template<class _T1, class _T2>
	inline bool operator<(const Pair<_T1, _T2>& __x, const Pair<_T1, _T2>& __y) {
		return __x.first < __y.first || (!(__y.first < __x.first) && __x.second < __y.second);
	}

	template<class _T1, class _T2>
	inline bool operator>(const Pair<_T1, _T2>& __x, const Pair<_T1, _T2>& __y) {
		return __y < __x;
	}

	template<class _T1, class _T2>
	inline Pair<_T1, _T2>
	make_pair(_T1 __x, _T2 __y) {
		return Pair<_T1, _T2>(__x, __y);
	}
  }
}

#endif /* end of include guard E3PAIR_H */

