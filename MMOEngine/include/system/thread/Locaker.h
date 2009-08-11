/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef MUTEX_H_
#define MUTEX_H_

#include "../platform.h"

#include <pthread.h>

#include "../lang/System.h"

#include "../lang/String.h"
#include "../lang/StackTrace.h"
#include "../lang/Time.h"

#include "Atomic.h"

namespace sys {
  namespace thread {

	class ReadWriteLock;

	class Mutex {
