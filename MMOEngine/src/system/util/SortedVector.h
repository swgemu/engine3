/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef SORTEDVECTOR_H_
#define SORTEDVECTOR_H_

#include "Vector.h"

namespace sys {
  namespace util {

	template<class E> class SortedVector : public Vector<E> {
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
		SortedVector(const SortedVector<E>& vector);

#ifdef CXX11_COMPILER
		SortedVector(std::initializer_list<E> l);
		SortedVector(SortedVector<E>&& vector);
#endif

		SortedVector<E>& operator=(const SortedVector<E>& vector);

#ifdef CXX11_COMPILER
		SortedVector<E>& operator=(SortedVector<E>&& vector);
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

	template<class E> SortedVector<E>::SortedVector() : Vector<E>() {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E> SortedVector<E>::SortedVector(int initsize, int incr) : Vector<E>(initsize, incr) {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class E> SortedVector<E>::SortedVector(const SortedVector<E>& vector) : Vector<E>(vector) {
		insertPlan = vector.insertPlan;
	}

#ifdef CXX11_COMPILER
	template<class E> SortedVector<E>::SortedVector(std::initializer_list<E> l) : Vector<E>(l.size(), 5) {
		insertPlan = ALLOW_DUPLICATE;

		for (auto it = l.begin(); it != l.end(); ++it) {
			put(*it);
		}
	}

	template<class E> SortedVector<E>::SortedVector(SortedVector<E>&& vector) : Vector<E>(std::move(vector)) {
		insertPlan = vector.insertPlan;
	}
#endif

	template<class E> SortedVector<E>& SortedVector<E>::operator=(const SortedVector<E>& vector) {
		if (this == &vector)
			return *this;

		insertPlan = vector.getInsertPlan();

		Vector<E>::operator=(vector);

		return *this;
	}

#ifdef CXX11_COMPILER
	template<class E> SortedVector<E>& SortedVector<E>::operator=(SortedVector<E>&& vector) {
		if (this == &vector)
			return *this;

		insertPlan = vector.getInsertPlan();

		Vector<E>::operator=(std::move(vector));

		return *this;
	}
#endif

	template<class E> int SortedVector<E>::lowerBound(const E& o) const {
		if (ArrayList<E>::size() == 0)
			return ArrayList<E>::npos;

		int l = 0, r = Vector<E>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E>::elementData[m];
			cmp = compare(obj, o);

			if (cmp > 0) {
				l = m + 1;

				if (r < l)
					return m < ArrayList<E>::size() - 1 ? m + 1 : ArrayList<E>::npos;
			} else {
				r = m - 1;

				if (r < l)
					return m;
			}
		}

		return ArrayList<E>::npos;
	}

	template<class E> int SortedVector<E>::upperBound(const E& o) const {
		if (ArrayList<E>::size() == 0)
			return ArrayList<E>::npos;

		int l = 0, r = Vector<E>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E>::elementData[m];
			cmp = compare(obj, o);

			if (cmp == 0 || cmp > 0) {
				l = m + 1;

				if (r < l)
					return m < ArrayList<E>::size() - 1 ? m + 1 : ArrayList<E>::npos;
			} else {
				r = m - 1;

				if (r < l)
					return m;
			}
		}

		return ArrayList<E>::npos;
	}

	template<class E> int SortedVector<E>::put(const E& o) {
		int m = 0, l = 0;
		int r = Vector<E>::elementCount - 1;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E>::elementData[m];
			int cmp = compare(obj, o);

			if (cmp == 0) {
				switch (insertPlan) {
					case ALLOW_DUPLICATE:
						Vector<E>::add(++m, o);
						break;
					case ALLOW_OVERWRITE:
						Vector<E>::set(m, o);
						break;
					default:
						return ArrayList<E>::npos;
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

		Vector<E>::add(m, o);

		return m;
	}
#ifdef CXX11_COMPILER
	template<class E> int SortedVector<E>::put(E&& o) {
		int m = 0, l = 0;
		int r = Vector<E>::elementCount - 1;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E>::elementData[m];
			int cmp = compare(obj, o);

			if (cmp == 0) {
				switch (insertPlan) {
					case ALLOW_DUPLICATE:
						if (std::is_move_constructible<E>::value)
							Vector<E>::add(++m, std::move(o));
						else
							Vector<E>::add(++m, o);
						break;
					case ALLOW_OVERWRITE:
						if (std::is_move_constructible<E>::value)
							Vector<E>::set(m, std::move(o));
						else
							Vector<E>::set(m, o);
						break;
					default:
						return ArrayList<E>::npos;
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
			Vector<E>::add(m, std::move(o));
		else
			Vector<E>::add(m, o);

		return m;
	}
#endif

	template<class E> bool SortedVector<E>::contains(const E& o) const {
		return find(o) != ArrayList<E>::npos;
	}

	template<class E> int SortedVector<E>::find(const E& o) const {
		if (ArrayList<E>::size() == 0)
			return ArrayList<E>::npos;

		int l = 0, r = Vector<E>::elementCount - 1;
		int m = 0, cmp = 0;

		while (l <= r) {
			//m = (l + r) / 2;
			m = ((unsigned int)l + (unsigned int)r) >> 1;

			const E& obj = Vector<E>::elementData[m];
			cmp = compare(obj, o);

			if (cmp == 0)
				return m;
			else if (cmp > 0)
				l = m + 1;
			else
				r = m - 1;
		}

		return ArrayList<E>::npos;
	}

	template<class E> Object* SortedVector<E>::clone() {
		return new SortedVector<E>(*this);
	}

	template<class E> bool SortedVector<E>::drop(const E& o) {
		int index = find(o);
		if (index == ArrayList<E>::npos)
			return false;

		//E& oldValue = Vector<E>::elementData[index];

		Vector<E>::remove(index);
		return true;
	}

  } // namespace util
} // namespace sys

#endif /*SORTEDVECTOR_H_*/
