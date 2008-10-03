/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#include "System.h"

MTRand System::mtrand;

PrintStream* System::out = new PrintStream();

uint32 System::random(unsigned int bucket) {
   	return mtrand.randInt(bucket);
}
