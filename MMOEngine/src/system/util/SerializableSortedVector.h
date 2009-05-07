/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef SERIALIZABLESORTEDVECTOR_H_
#define SERIALIZABLESORTEDVECTOR_H_

#include "SerializableVector.h"

namespace sys {
	namespace util {

	template<class O> class SerializableSortedVector : public SerializableVector<O> {
		int insertPlan;

	public:
		static const int ALLOW_DUPLICATE = 1;

		static const int NO_DUPLICATE = 2;

	public:
		SerializableSortedVector();
		SerializableSortedVector(int initsize, int incr);

		int put(const O& obj);

		bool contains(const O& obj) const;

		int find(const O& obj) const;

		bool drop(const O& obj);

		inline void setInsertPlan(int plan) {
			insertPlan = plan;
		}

	};

	template<class O> SerializableSortedVector<O>::SerializableSortedVector() : SerializableVector<O> () {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class O> SerializableSortedVector<O>::SerializableSortedVector(int initsize, int incr)
	: SerializableVector<O>(initsize, incr) {
		insertPlan = ALLOW_DUPLICATE;
	}

	template<class O> int SerializableSortedVector<O>::put(const O& obj) {
		int m = 0, l = 0;
		int r = SerializableVector<O> ::elementCount - 1;

		while (l <= r) {
			m = (l + r) / 2;

			O temp = SerializableVector<O>::elementData[m];
			int cmp = temp.compareTo(obj);

			if (cmp == 0) {
				if (insertPlan == ALLOW_DUPLICATE)
					SerializableVector<O>::add(++m, obj);
				else
					return -1;

				return m;
			} else if (cmp > 0)
				l = m + 1;
			else
				r = m - 1;
		}

		if (r == m)
			m++;

		SerializableVector<O>::add(m, obj);

		return m;
	}

	template<class O> bool SerializableSortedVector<O>::contains(const O& obj) const {
		return find(obj) != -1;
	}

	template<class O> int SerializableSortedVector<O>::find(const O& obj) const {
		int l = 0, r = SerializableVector<O>::elementCount - 1;
		int m = 0;

		while (l <= r) {
			m = (l + r) / 2;

			O temp = SerializableVector<O>::elementData[m];
			int cmp = temp.compareTo(obj);

			if (cmp == 0)
				return m;
			else if (cmp > 0)
				l = m + 1;
			else
				r = m - 1;
		}

		return -1;
	}

	template<class O> bool SerializableSortedVector<O>::drop(const O& obj) {
		int index = find(obj);
		if (index == -1)
			return false;

		O oldValue = SerializableVector<O>::elementData[index];

		SerializableVector<O>::remove(index);
		return true;
	}

	}
}


#endif /* SERIALIZABLESORTEDVECTOR_H_ */
