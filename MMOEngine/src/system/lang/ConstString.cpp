/**
 * @author      : theanswer (theanswer@hyperv)
 * @file        : ConstString
 * @created     : Monday Sep 02, 2019 12:18:09 UTC
 */

#include "ConstString.h"
#include "String.h"

using namespace sys::lang;

constexpr uint32 sys::lang::ConstString::crctable[];

String ConstString::toString() const {
	return String(p_, sz_);
}


