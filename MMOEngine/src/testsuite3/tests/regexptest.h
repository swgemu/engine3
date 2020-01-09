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

#include <regex>

#include "engine/engine.h"

int regexptest() {
	UnicodeString text = "\\>";

	int index;

	int count = 10;
	while ((index = text.indexOf("\\>")) >= 0 && --count > 0) {
		printf("%d\n", index);
		text = text.replaceFirst("\\>", "");
	}

	return 0;



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



	std::string l = testString;
		
	std::regex word_regex(regexString.toCharArray(), std::regex_constants::extended);

	std::smatch match;
	if (std::regex_search(l, match, word_regex)) {
		printf("found std::regex!\n");

		printf("prefix length %d\n", match.prefix().length());
		printf("suffix length %d\n", match[0].second - match[0].first);

	}

	return 0;
}


#endif /* REGEXPTEST_H_ */
