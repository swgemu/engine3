//
// Created by Victor Popovici on 4/2/18.
//

#include "json_utils.h"

#include "system/lang/String.h"
#include "system/lang/UnicodeString.h"

void sys::lang::to_json(nlohmann::json& j, const sys::lang::String& p) {
	j = p.toCharArray();
}

void sys::lang::to_json(nlohmann::json& j, const sys::lang::UnicodeString& p) {
	j = p.toString().toCharArray();
}

void sys::lang::to_json(nlohmann::json& j, const sys::lang::Time& p) {
	j = p.getMiliTime();
}