/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/

#ifndef TESTCLASS_H_
#define TESTCLASS_H_

#ifdef WITH_STM
class TestClass : public Object {
	static const int ELEMENT_COUNT = 10;

	int values[ELEMENT_COUNT];

public:
	TestClass(int value) {
		for (int i = 0; i < ELEMENT_COUNT; ++i)
			values[i] = value;
	}

	TestClass(TestClass& obj) : Object() {
		for (int i = 0; i < ELEMENT_COUNT; ++i)
			values[i] = obj.values[i];
	}

	void increment() {
		for (int i = 0; i < ELEMENT_COUNT; ++i)
			values[i] += 1;
	}

	int get() {
		int value = values[0];

		for (int i = 0; i < ELEMENT_COUNT; ++i) {
			if (values[i] != value)
				assert(0 && "inconsistency in object");
		}

		return value;
	}

	int getz() {
		int value = values[0];

		return value;
	}

	Object* clone() {
		return ObjectCloner<TestClass>::clone(this);
	}

	Object* clone(void* object) {
		return TransactionalObjectCloner<TestClass>::clone(this);
	}

	void free() {
		TransactionalMemoryManager::instance()->destroy(this);
	}
};

#endif

#endif /* TESTCLASS_H_ */
