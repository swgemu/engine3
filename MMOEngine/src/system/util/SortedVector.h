/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SORTEDVECTOR_H_
#define SORTEDVECTOR_H_

#include "Vector.h"

namespace sys {
  namespace util {

	template<class E, bool RawCopyAndRealloc = ArrayListReallocTrait<E>::value>
	class SortedVector : public Vector<E, RawCopyAndRealloc> {
	protected:
		int insertPlan;

		virtual int compare(const E& o1, const E& o2) const {
			return TypeInfo<E>::compare(o1, o2);
		}

	public:
		static const int ALLOW_DUPLICATE = 1;

		static const int NO_DUPLICATE = 2;

		static const int ALLOW_OVERWRITE = 3;

	public:
		SortedVector();
		SortedVector(int initsize, int incr);
		SortedVector(const SortedVector<E, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
		SortedVector(std::initializer_list<E> l);
		SortedVector(SortedVector<E, RawCopyAndRealloc>&& vector);
#endif

		SortedVector<E, RawCopyAndRealloc>& operator=(const SortedVector<E, RawCopyAndRealloc>& vector);

#ifdef CXX11_COMPILER
		SortedVector<E, RawCopyAndRealloc>& operator=(SortedVector<E, RawCopyAndRealloc>&& vector);
#endif

		virtual int put(const E& o);

#ifdef CXX11_COMPILER
		virtual int put(E&& o);
#endif

		virtual int find(const E& o) const;

		virtual bool contains(const E& o) const;

		int lowerBound(const E& o) const;
		int upperBound(const E& o) const;

		virtual bool drop(const E& o);

		Object* clone();

		inline void setInsertPlan(int plan) {
			insertPlan = plan;
		}

		inline void setNoDuplicateInsertPlan() {
			insertPlan = NO_DUPLICATE;
		}

		inline void setAllowDuplicateInsertPlan() {
			insertPlan = ALLOW_DUPLICATE;
		}

		inline void setAllowOverwriteInsertPlan() {
			insertPlan = ALLOW_OVERWRITE;
		}

		inline int getInsertPlan() const {
			return insertPlan;
		}

	};

	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>::SortedVector() : Vector<E, RawCopyAndRealloc>() {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>::SortedVector(int initsize, int incr) : Vector<E, RawCopyAndRealloc>(initsize, incr) {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>::SortedVector(const SortedVector<E, RawCopyAndRealloc>& vector) : Vector<E, RawCopyAndRealloc>(vector) {
		insertPlan = vector.insertPlan;
	}

#ifdef CXX11_COMPILER
	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>::SortedVector(std::initializer_list<E> l) : Vector<E, RawCopyAndRealloc>(l.size(), 5) {
		insertPlan = ALLOW_DUPLICATE;

		for (auto it = l.begin(); it != l.end(); ++it) {
			put(*it);
		}
	}

	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>::SortedVector(SortedVector<E, RawCopyAndRealloc>&& vector) : Vector<E, RawCopyAndRealloc>(std::move(vector)) {
		insertPlan = vector.insertPlan;
	}
#endif

	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>& SortedVector<E, RawCopyAndRealloc>::operator=(const SortedVector<E, RawCopyAndRealloc>& vector) {
		if (this == &vector)
			return *this;

		insertPlan = vector.getInsertPlan();

		Vector<E, RawCopyAndRealloc>::operator=(vector);

		return *this;
	}

#ifdef CXX11_COMPILER
	template<class E, bool RawCopyAndRealloc> SortedVector<E, RawCopyAndRealloc>& SortedVector<E, RawCopyAndRealloc>::operator=(SortedVector<E, RawCopyAndRealloc>&& vector) {
		if (this == &vector)
			return *this;

		insertPlan = vector.getInsertPlan();

		Vector<E, RawCopyAndRealloc>::operator=(std::move(vector));

		return *this;
	}
#endif

	template<class E, bool RawCopyAndRealloc> int SortedVector<E, RawCopyAndRealloc>::lowerBound(const E& o) const {
		if (ArrayList<E, RawCopyAndRealloc>::size() == 0)
			return ArrayList<E, RawCopyAndRealloc>::npos;

		int l = 0, r = Vector<E, RawCopyAndRealloc>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			m = ((uint32)l + (uint32)r) >> 1;

			const E& obj = Vector<E, RawCopyAndRealloc>::elementData[m];
			cmp = compare(obj, o);

			if (cmp > 0) {
				l = m + 1;

				if (r < l)
					return m < ArrayList<E, RawCopyAndRealloc>::size() - 1 ? m + 1 : ArrayList<E, RawCopyAndRealloc>::npos;
			} else {
				r = m - 1;

				if (r < l)
					return m;
			}
		}

		return ArrayList<E, RawCopyAndRealloc>::npos;
	}

	template<class E, bool RawCopyAndRealloc> int SortedVector<E, RawCopyAndRealloc>::upperBound(const E& o) const {
		if (ArrayList<E, RawCopyAndRealloc>::size() == 0)
			return ArrayList<E, RawCopyAndRealloc>::npos;

		int l = 0, r = Vector<E, RawCopyAndRealloc>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			m = ((uint32)l + (uint32)r) >> 1;

			const E& obj = Vector<E, RawCopyAndRealloc>::elementData[m];
			cmp = compare(obj, o);

			if (cmp == 0 || cmp > 0) {
				l = m + 1;

				if (r < l)
					return m < ArrayList<E, RawCopyAndRealloc>::size() - 1 ? m + 1 : ArrayList<E, RawCopyAndRealloc>::npos;
			} else {
				r = m - 1;

				if (r < l)
					return m;
			}
		}

		return ArrayList<E, RawCopyAndRealloc>::npos;
	}

	template<class E, bool RawCopyAndRealloc> int SortedVector<E, RawCopyAndRealloc>::put(const E& o) {
		int m = 0, l = 0;
		int r = Vector<E, RawCopyAndRealloc>::elementCount - 1;

		while (l <= r) {
			m = ((uint32)l + (uint32)r) >> 1;

			const E& obj = Vector<E, RawCopyAndRealloc>::elementData[m];
			int cmp = compare(obj, o);

			if (cmp == 0) {
				switch (insertPlan) {
					case ALLOW_DUPLICATE:
						Vector<E, RawCopyAndRealloc>::add(++m, o);
						break;
					case ALLOW_OVERWRITE:
						Vector<E, RawCopyAndRealloc>::set(m, o);
						break;
					default:
						return ArrayList<E, RawCopyAndRealloc>::npos;
				}

				return m;
			} else if (cmp > 0) {
				l = m + 1;
			} else {
				r = m - 1;
			}
		}

		if (r == m)
			m++;

		Vector<E, RawCopyAndRealloc>::add(m, o);

		return m;
	}
#ifdef CXX11_COMPILER
	template<class E, bool RawCopyAndRealloc> int SortedVector<E, RawCopyAndRealloc>::put(E&& o) {
		int m = 0, l = 0;
		int r = Vector<E, RawCopyAndRealloc>::elementCount - 1;

		while (l <= r) {
			m = ((uint32)l + (uint32)r) >> 1;

			const E& obj = Vector<E, RawCopyAndRealloc>::elementData[m];
			int cmp = compare(obj, o);

			if (cmp == 0) {
				switch (insertPlan) {
					case ALLOW_DUPLICATE:
						if (std::is_move_constructible<E>::value)
							Vector<E, RawCopyAndRealloc>::add(++m, std::move(o));
						else
							Vector<E, RawCopyAndRealloc>::add(++m, o);
						break;
					case ALLOW_OVERWRITE:
						if (std::is_move_constructible<E>::value)
							Vector<E, RawCopyAndRealloc>::set(m, std::move(o));
						else
							Vector<E, RawCopyAndRealloc>::set(m, o);
						break;
					default:
						return ArrayList<E, RawCopyAndRealloc>::npos;
				}

				return m;
			} else if (cmp > 0) {
				l = m + 1;
			} else {
				r = m - 1;
			}
		}

		if (r == m)
			m++;

		if (std::is_move_constructible<E>::value)
			Vector<E, RawCopyAndRealloc>::add(m, std::move(o));
		else
			Vector<E, RawCopyAndRealloc>::add(m, o);

		return m;
	}
#endif

	template<class E, bool RawCopyAndRealloc> bool SortedVector<E, RawCopyAndRealloc>::contains(const E& o) const {
		return find(o) != ArrayList<E, RawCopyAndRealloc>::npos;
	}

	template<class E, bool RawCopyAndRealloc> int SortedVector<E, RawCopyAndRealloc>::find(const E& o) const {
		if (ArrayList<E, RawCopyAndRealloc>::size() == 0)
			return ArrayList<E, RawCopyAndRealloc>::npos;

		int l = 0, r = Vector<E, RawCopyAndRealloc>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E, RawCopyAndRealloc>::elementData[m];
			cmp = compare(obj, o);

			if (cmp == 0)
				return m;
			else if (cmp > 0)
				l = m + 1;
			else
				r = m - 1;
		}

		return ArrayList<E, RawCopyAndRealloc>::npos;
	}

	template<class E, bool RawCopyAndRealloc> Object* SortedVector<E, RawCopyAndRealloc>::clone() {
		return new SortedVector<E, RawCopyAndRealloc>(*this);
	}

	template<class E, bool RawCopyAndRealloc> bool SortedVector<E, RawCopyAndRealloc>::drop(const E& o) {
		int index = find(o);
		if (index == ArrayList<E, RawCopyAndRealloc>::npos)
			return false;

		//E& oldValue = Vector<E, RawCopyAndRealloc>::elementData[index];

		Vector<E, RawCopyAndRealloc>::remove(index);
		return true;
	}

  } // namespace util
} // namespace sys

#endif /*SORTEDVECTOR_H_*/
