/*
 * lowerbound.h
 *
 *  Created on: 04/05/2012
 *      Author: victor
 */

#ifndef LOWERBOUND_H_
#define LOWERBOUND_H_

void lowerBoundTest() {
	SortedVector<String> vec;
	/*vec.put(1);
	vec.put(2);
	vec.put(3);
	vec.put(4);*/
	vec.put("Alfa");
	vec.put("AlfaBeta");
	vec.put("AlfaBetaGamma");

	//int search = 4;

	String search("AlfaBetaGamma");

	int low = vec.lowerBound(search);
	int up = vec.upperBound(search);


	printf("lower bound:%d upper bound:%d\n", low, up);
}


#endif /* LOWERBOUND_H_ */
