/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * regexptest.h
 *
 *  Created on: 28/08/2013
 *      Author: victor
 */

#ifndef REGEXPTEST_H_
#define REGEXPTEST_H_

#include "engine/engine.h"

int regexptest() {
	String testString = "testing\n stuff #1dr #asdf";
	String simpleString = "#";
	String regexString = "\n|\r|#";

	/*int res = testString.indexOf(regexString);
	int res2 = testString.indexOf("#");
	char* bla = strstr(testString.toCharArray(), "#");
	int res3 = bla - testString.toCharArray();*/

	String replaced = testString.replaceFirst(regexString, "");
	String replacedAll = testString.replaceAll(regexString, "");

	//printf("regexp return res:%d res2:%d res3:%d replaced:%s\n", res, res2, res3, replaced.toCharArray());

	printf("pre replace string:%s\n ", testString.toCharArray());
	printf("regexp return replaceFirst:%s\n", replaced.toCharArray());
	printf("regexp return replaceAll:%s\n", replacedAll.toCharArray());



	return 0;
}


#endif /* REGEXPTEST_H_ */
