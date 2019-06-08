/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
/*
 * lowerbound.h
 *
 *  Created on: 04/05/2012
 *      Author: victor
 */

#ifndef LOWERBOUND_H_
#define LOWERBOUND_H_

#include <map>
#include <string>
#include <iostream>

void lowerBoundTest() {
	VectorMap<String, int> auctionItems;
	auctionItems.setNoDuplicateInsertPlan();
	auctionItems.put("tatooine.bestine.vendor.1", 1000);
	auctionItems.put("tatooine.bestine.vendor.2", 2000);
	auctionItems.put("tatooine.bestine.bazaar.1", 3000);
	auctionItems.put("tatooine.bestine.bazaar.2", 4000);
	auctionItems.put("tatooine.bestine.bazaar.3", 5000);
	auctionItems.put("tatooine.bestine.bazaar.4", 6000);
	auctionItems.put("tatooine.mos_taike.vendor.1", 7000);
	auctionItems.put("tatooine.mos_taike.vendor.2", 8000);
	auctionItems.put("tatooine.mos_taike.vendor.3", 9000);
	auctionItems.put("tatooine.mos_taike.bazaar.1", 10000);
	auctionItems.put("tatooine.mos_taike.bazaar.2", 11000);
	auctionItems.put("tatooine.mos_eisley.vendor.1", 12000);
	auctionItems.put("tatooine.mos_eisley.vendor.2", 13000);
	auctionItems.put("tatooine.mos_eisley.bazaar.1", 14000);
	auctionItems.put("tatooine.mos_eisley.bazaar.2", 15000);
	auctionItems.put("tatooine.wayfar.vendor.1", 16000);
	auctionItems.put("tatooine.wayfar.bazaar.1", 17000);
	auctionItems.put("tatooine.wayfar.bazaar.2", 18000);

	std::map<std::string, int> testMap;
//	auctionItems.setNoDuplicateInsertPlan();
	std::string test("tatooine.bestine.vendor.1");

	testMap[test] = 1000;
	testMap[std::string("tatooine.bestine.vendor.2")] = 2000;
	testMap[std::string("tatooine.bestine.bazaar.1")] = 3000;
	testMap[std::string("tatooine.bestine.bazaar.2")] = 4000;
	testMap[std::string("tatooine.bestine.bazaar.3")] = 5000;
	testMap[std::string("tatooine.bestine.bazaar.4")] = 6000;
	testMap[std::string("tatooine.mos_taike.vendor.1")] = 7000;
	testMap[std::string("tatooine.mos_taike.vendor.2")] = 8000;
	testMap[std::string("tatooine.mos_taike.vendor.3")] = 9000;
	testMap[std::string("tatooine.mos_taike.bazaar.1")] = 10000;
	testMap[std::string("tatooine.mos_taike.bazaar.2")] = 11000;
	testMap[std::string("tatooine.mos_eisley.vendor.1")] = 12000;
	testMap[std::string("tatooine.mos_eisley.vendor.2")] = 13000;
	testMap[std::string("tatooine.mos_eisley.bazaar.1")] = 14000;
	testMap[std::string("tatooine.mos_eisley.bazaar.2")] = 15000;
	testMap[std::string("tatooine.wayfar.vendor.1")] = 16000;
	testMap[std::string("tatooine.wayfar.bazaar.1")] = 17000;
	testMap[std::string("tatooine.wayfar.bazaar.2")] = 18000;

	for (int i = 0; i < auctionItems.size(); ++i) {
		printf("%d: %s\n", i, auctionItems.elementAt(i).getKey().toCharArray());
	}

	String search = "tatooine.bestine";

	int low = auctionItems.lowerBound(search);
	int up = auctionItems.upperBound(search);

	std::map<std::string,int>::iterator it,itlow,itup;
	itlow = testMap.lower_bound("tatooine.bestine");
	itup = testMap.upper_bound("tatooine.bestine");


	printf("lower bound:%d upper bound:%d\n", low, up);
	printf("std lower bound:%s upper bound:%s\n", (*itlow).first.c_str(), (*itup).first.c_str());
}


#endif /* LOWERBOUND_H_ */
