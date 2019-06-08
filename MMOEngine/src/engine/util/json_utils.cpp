/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 4/2/18.
//

#include "json_utils.h"

#include "system/lang/String.h"
#include "system/lang/UnicodeString.h"

#include "system/thread/atomic/AtomicInteger.h"

#include "engine/util/u3d/Vector3.h"
#include "engine/util/u3d/Quaternion.h"
#include "engine/util/u3d/Coordinate.h"

void sys::lang::to_json(nlohmann::json& j, const sys::lang::String& p) {
	j = p.toCharArray();
}

void sys::lang::to_json(nlohmann::json& j, const sys::lang::UnicodeString& p) {
	j = p.toString().toCharArray();
}

void sys::lang::to_json(nlohmann::json& j, const sys::lang::Time& p) {
	j = p.getMiliTime();
}

void sys::thread::atomic::to_json(nlohmann::json& j, const sys::thread::atomic::AtomicInteger& p) {
	j = p.get();
}

void engine::util::u3d::to_json(nlohmann::json& j, const engine::util::u3d::Vector3& v) {
	j["x"] = v.getX();
	j["y"] = v.getY();
	j["z"] = v.getZ();
}

void engine::util::u3d::to_json(nlohmann::json& j, const engine::util::u3d::Quaternion& v) {
	j["x"] = v.getX();
	j["y"] = v.getY();
	j["z"] = v.getZ();
	j["w"] = v.getW();
}

void engine::util::u3d::to_json(nlohmann::json& j, const engine::util::u3d::Coordinate& v) {
	j["position"] = v.getPosition();
	j["previousPosition"] = v.getPreviousPosition();
}
