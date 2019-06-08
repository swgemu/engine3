/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * randomtest.h
 *
 *  Created on: 17/07/2013
 *      Author: victor
 */

#ifndef RANDOMTEST_H_
#define RANDOMTEST_H_


void randomTest() {
	HashTable<uint32, uint32> values;
	values.put(0, 0);
	values.put(1, 0);

	for (uint32 i = 0; i < 1000000000; ++i) {
		int res = System::random(1);

		uint32 count = values.get(res);
		values.put(res, count + 1);
	}

	printf("0 count:%d 1 count:%d", values.get(0), values.get(1));
}


#endif /* RANDOMTEST_H_ */
