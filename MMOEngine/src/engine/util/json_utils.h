//
// Created by Victor Popovici on 4/2/18.
//

#ifndef ENGINE3_JSON_UTILS_H
#define ENGINE3_JSON_UTILS_H

#include "json.hpp"

#include "system/util/VectorMap.h"

namespace sys {
	namespace lang {
		class String;
		class UnicodeString;

		void to_json(nlohmann::json& j, const sys::lang::String& p);
		void to_json(nlohmann::json& j, const sys::lang::UnicodeString& p);
		void to_json(nlohmann::json& j, const sys::lang::Time& p);
	}
}

namespace sys {
	namespace util {
	 template<class K, class V>
	 void to_json(nlohmann::json& j, const VectorMapEntry<K, V>& entry) {
		 using json = nlohmann::json;

		 j = {{entry.getKey(), entry.getValue()}};
	 }

	 template<typename V>
	 void to_json(nlohmann::json& j, const ArrayList<V>& p) {
		 using json = nlohmann::json;

		 j = json::array();

		 for (const auto& entry : p) {
			 j.push_back(entry);
		 }
	 }

 }
}

#endif //ENGINE3_JSON_UTILS_H
