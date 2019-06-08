/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
//
// Created by Victor Popovici on 4/2/18.
//

#pragma once

#include "json.hpp"

#include "system/util/VectorMap.h"
#include "system/util/HashTable.h"
#include "system/util/HashSet.h"
#include "system/util/SynchronizedVectorMap.h"
#include "system/util/SynchronizedSortedVector.h"

namespace engine {
	namespace core {
		template<class O> class ManagedReference;
		template<class O> class ManagedWeakReference;

		template<class O>
		void to_json(nlohmann::json& j, const engine::core::ManagedReference<O>& p) {
			j = p.getObjectID();
		}

		template<class O>
		void to_json(nlohmann::json& j, const engine::core::ManagedWeakReference<O>& p) {
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

	 template<typename K, typename V>
	 void to_json(nlohmann::json& j, const HashTable<K, V>& p) {
		 using json = nlohmann::json;

		 j = json::array();

		 auto iterator = p.iterator();

		 while (iterator.hasNext()) {
			K* key;
			V* value;

		 	iterator.getNextKeyAndValue(key, value);

			j.push_back({{*key, *value}});
		 }
	 }

	 template<typename K, typename V>
	 void to_json(nlohmann::json& j, const SynchronizedVectorMap<K, V>& p) {
	 	j = p.getMapUnsafe();
	 }

	 template<typename V>
	 void to_json(nlohmann::json& j, const SynchronizedVector<V>& v) {
	 	j = v.getVectorUnsafe();
	 }

	 template<typename K>
	 void to_json(nlohmann::json& j, const HashSet<K>& p) {
		using json = nlohmann::json;

		j = json::array();

		auto iterator = p.iterator();

		while (iterator.hasNext()) {
			const K& key = iterator.next();

			j.push_back(key);
		}
	 }
 }
}

namespace engine {
	namespace util {
		namespace u3d {
			class Vector3;
			class Quaternion;
			class Coordinate;

			void to_json(nlohmann::json& k, const engine::util::u3d::Vector3& v);
			void to_json(nlohmann::json& k, const engine::util::u3d::Quaternion& q);
			void to_json(nlohmann::json& k, const engine::util::u3d::Coordinate& q);
		}
	}
}

namespace sys {
	namespace thread {
		namespace atomic {
			class AtomicInteger;

			void to_json(nlohmann::json& j,	const sys::thread::atomic::AtomicInteger& p);
		}
	}
}

