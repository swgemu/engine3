//
// Created by Victor Popovici on 4/2/18.
//

#ifndef ENGINE3_JSON_UTILS_H
#define ENGINE3_JSON_UTILS_H

#include "json.hpp"

#include "system/util/VectorMap.h"
#include "engine/core/ManagedReference.h"
#include "engine/core/ManagedWeakReference.h"

namespace engine {
	namespace core {
		template<class V>
		void to_json(nlohmann::json& j, const ManagedReference<V>& p) {
			if (p.get() == nullptr) {
				j = 0;
			} else {
				j = p->_getObjectID();
			}
		}

		template<class V>
		void to_json(nlohmann::json& j, const ManagedWeakReference<V>& p) {
			j = p.getSavedObjectID();
		}
	}
}

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
