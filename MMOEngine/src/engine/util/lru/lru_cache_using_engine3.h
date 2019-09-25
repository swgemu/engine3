/*
** Copyright (C) 2007-2019 SWGEmu
** See file COPYING for copying conditions.
*/
#ifndef LRU_CACHE_USING_ENGINE3_H_
#define LRU_CACHE_USING_ENGINE3_H_
#include "system/util/SynchronizedHashTable.h"

#include <utility>
#include <cassert>
#include <list>

// Class providing fixed-size (by number of records)
// LRU-replacement cache of a function with signature
// V f(K).

template<typename K, typename V>
class LRUFunction {
public:
	virtual ~LRUFunction() {

	}

	virtual V run(const K& k) = 0;
};

template<typename K, typename Arg1, typename Arg2, typename V>
class LRUFunction2 {
public:
	virtual ~LRUFunction2() {

	}

	virtual V run(const Arg1& k, const Arg2& k2) = 0;

	virtual K hash(const Arg1& k, const Arg2& k2) const = 0;
};

template<typename A, typename B>
class LRUCacheEntry {
	A key;
	B value;
	std::atomic<uint32> accessCount;
public:
	LRUCacheEntry() : accessCount(0) {

	}

	LRUCacheEntry(const A& a, const B& b) : key(a), value(b), accessCount(0) {

	}

#ifdef CXX11_COMPILER
	LRUCacheEntry(const LRUCacheEntry& a) : key(a.key), value(a.value), accessCount(a.accessCount.load(std::memory_order_relaxed)) {
	}

	LRUCacheEntry(LRUCacheEntry&& a) : key(std::move(a.key)),
			value(std::move(a.value)), accessCount(a.accessCount.load(std::memory_order_relaxed)) {
	}

	LRUCacheEntry& operator=(LRUCacheEntry&& a) {
		if (this == &a)
			return *this;

		key = std::move(a.key);
		value = std::move(a.value);

		accessCount.store(a.accessCount.load(std::memory_order_relaxed));

		return *this;
	}
#endif
	LRUCacheEntry& operator=(const LRUCacheEntry& a) {
		if (this == &a)
			return *this;

		key = a.key;
		value = a.value;

		accessCount.store(a.accessCount.load(std::memory_order_relaxed));

		return *this;
	}

	A& first() {
		return key;
	}

	B& second() {
		return value;
	}

	const A& first() const {
		return key;
	}

	const B& second() const {
		return value;
	}

	bool incrementAccessCount(int max) {
		uint32 old, newCount;
		bool result;

		do {
			result = false;
			old = accessCount.load(std::memory_order_relaxed);

			newCount = old + 1;

			if (newCount == max) {
				newCount = 0;

				result = true;
			}
		} while (!accessCount.compare_exchange_strong(old, newCount));

		return result;
	}

	uint32 getAccessCount() const {
		return accessCount.load(std::memory_order_relaxed);
	}

	bool toBinaryStream(ObjectOutputStream* stream) {
		return false;
	}

	bool parseFromBinaryStream(ObjectInputStream* stream) {
		return false;
	}

};

template<typename A, typename B>
LRUCacheEntry<A, B> custom_make_pair(const A& a, const B& b) {
	return LRUCacheEntry<A, B>(a, b);
}

/*
template<typename ReturnValue, typename Argument> ReturnValue CreateLRUKey(const Argument& arg) {
	return arg;
}
*/

template <typename K, typename V>
class lru_cache_using_engine3 {
public:

  typedef K key_type;
  typedef V value_type;

  typedef LRUFunction<key_type, value_type>* function_type;

  // Key access history, most recent at back
  typedef std::list<key_type> key_tracker_type;
  typedef typename key_tracker_type::iterator key_traker_iterator_type;

  typedef LRUCacheEntry<value_type,
		  key_traker_iterator_type> map_value_type;

  // Key to value and key history iterator
  typedef SynchronizedHashTable<key_type,
		  map_value_type> key_to_value_type;

  // Constuctor specifies the cached function and
  // the maximum number of records to be stored
  lru_cache_using_engine3(function_type func, size_t c, int accessCountToPromote = 5)
    	:_fn(func), _capacity(c), minAccessCountForPromoting(accessCountToPromote) {
    E3_ASSERT(_capacity != 0);
    E3_ASSERT(minAccessCountForPromoting > 0);
  }

  virtual ~lru_cache_using_engine3() {
	  delete _fn;
  }

  void clear() {
	  Locker locker(&_key_to_value);

	  _key_to_value.removeAll();
	  _key_tracker.clear();

	  hitCount = 0;
	  missCount = 0;
  }

  // Obtain value of the cached function for k
  virtual value_type operator()(const key_type& k) {
	  // Attempt to find existing record
	  ReadLocker readLocker(&_key_to_value);

	  Entry<K, map_value_type>* entry = _key_to_value.getEntryUnsafe(k);

	  if (entry == nullptr) {// We don't have it:
		  // Evaluate function and create new record
		  readLocker.release();

		  missCount.increment();

		  const value_type v = _fn->run(k);

		  insert(k,v);

		  // Return the freshly computed value
		  return v;
	  } else { // We do have it:
		  hitCount.increment();

		  value_type value = entry->getValue().first();

		  // Update access record by moving
		  // accessed key to back of list
		  const bool promote = entry->getValue().incrementAccessCount(minAccessCountForPromoting);

		  //const key_traker_iterator_type it = entry->getValue().second();

		  if (promote) { //promote only once(when hitting count)
			  readLocker.release();

			  Locker writeLocker(&_key_to_value);

			  //We need to search for the entry again because a different thread might have updated
			  //it while we were releasing the read lock and acquiring the write lock
			  entry = _key_to_value.getEntryUnsafe(k);

			  if (entry != nullptr) {
				  //safe by the write lock to the map
				  const key_traker_iterator_type it = entry->getValue().second();

				  _key_tracker.splice(_key_tracker.end(), _key_tracker, it);
			  }

			  return value;
		  } else {
			  // Return the retrieved value
			  return value;
		  }
	  }
  }

  int getHitCount() const {
	  return hitCount.get();
  }

  int getMissCount() const {
	  return missCount.get();
  }

  ReadWriteLock* getLock() {
	  return &_key_to_value;
  }

  virtual void remove(const key_type& hash) {
	  Locker writeLock(&_key_to_value);

	  if (!_key_to_value.containsKey(hash))
		  return;

	  map_value_type entry = _key_to_value.remove(hash);

	  const key_traker_iterator_type it = entry.second();

	  _key_tracker.erase(it);
  }

  int getSize() const {
	  return _key_to_value.size();
  }


protected:
  // Record a fresh key-value pair in the cache
  virtual bool insert(const key_type& k,const value_type& v) {
	  Locker locker(&_key_to_value);

	  // Method is only called on cache misses
	  //assert(_key_to_value.find(k) == _key_to_value.end());

	  if (_key_to_value.containsKey(k))
		  return false;

	  // Make space if necessary
	  if (_key_to_value.size() >= _capacity)
		  evict();

	  // Record k as most-recently-used key
	  key_traker_iterator_type it = _key_tracker.insert(_key_tracker.end(), k);

	  // Create the key-value entry,
	  // linked to the usage record.
	  _key_to_value.put(k, custom_make_pair(v, it));
	  // No need to check return,
	  // given previous assert.

	  return true;
  }

  // Purge the least-recently-used element in the cache
  virtual value_type evict() {
	  // Assert method is never called when cache is empty
	  E3_ASSERT(!_key_tracker.empty());

	  // Identify least recently used key
	  // Erase both elements to completely purge record
	  map_value_type ret = _key_to_value.remove(_key_tracker.front());
	  _key_tracker.pop_front();

	  return ret.first();
  }

protected:
  // The function to be cached
  //value_type (*_fn)(const key_type&);
  function_type _fn;

  // Maximum number of key-value pairs to be retained
  const size_t _capacity;

  // Key access history
  key_tracker_type _key_tracker;

  // Key-to-value lookup
  key_to_value_type _key_to_value;

  const int minAccessCountForPromoting;

  AtomicInteger hitCount, missCount;
};


template <typename K, typename Arg1, typename Arg2, typename V>
class lru_cache_using_engine3_2args {
public:

  typedef K key_type;
  typedef V value_type;

  typedef LRUFunction2<K, Arg1, Arg2, value_type>* function_type;

  // Key access history, most recent at back
  typedef std::list<key_type> key_tracker_type;
  typedef typename key_tracker_type::iterator key_traker_iterator_type;

  typedef LRUCacheEntry<value_type,
		  key_traker_iterator_type> map_value_type;

  // Key to value and key history iterator
  typedef SynchronizedHashTable<key_type,
		  map_value_type> key_to_value_type;

  // Constuctor specifies the cached function and
  // the maximum number of records to be stored
  lru_cache_using_engine3_2args(function_type func, size_t c, int accessCountToPromote = 5)
    	:_fn(func), _capacity(c), minAccessCountForPromoting(accessCountToPromote) {
    E3_ASSERT(_capacity != 0);
    E3_ASSERT(minAccessCountForPromoting > 0);
  }

  virtual ~lru_cache_using_engine3_2args() {
	  delete _fn;
  }

  void clear() {
	  Locker locker(&_key_to_value);

	  _key_to_value.removeAll();
	  _key_tracker.clear();

	  hitCount = 0;
	  missCount = 0;
  }

  // Obtain value of the cached function for k
  virtual value_type operator()(const Arg1& k, const Arg2& k2) {
	  // Attempt to find existing record
	  key_type hash = _fn->hash(k, k2);

	  ReadLocker readLocker(&_key_to_value);

	  Entry<key_type, map_value_type>* entry = _key_to_value.getEntryUnsafe(hash);

	  if (entry == nullptr) {// We don't have it:
		  // Evaluate function and create new record
		  readLocker.release();

		  missCount.increment();

		  const value_type v = _fn->run(k, k2);

		  insert(k, k2, v);

		  // Return the freshly computed value
		  return v;
	  } else { // We do have it:
		  hitCount.increment();

		  value_type value = entry->getValue().first();

		  // Update access record by moving
		  // accessed key to back of list
		  const bool promote = entry->getValue().incrementAccessCount(minAccessCountForPromoting);

		  //const key_traker_iterator_type it = entry->getValue().second();

		  if (promote) { //promote only once(when hitting count)
			  readLocker.release();

			  Locker writeLocker(&_key_to_value);

			  //We need to search for the entry again because a different thread might have updated
			  //it while we were releasing the read lock and acquiring the write lock
			  entry = _key_to_value.getEntryUnsafe(hash);

			  if (entry != nullptr) {
				  //safe by the write lock to the map
				  const key_traker_iterator_type it = entry->getValue().second();

				  _key_tracker.splice(_key_tracker.end(), _key_tracker, it);
			  }

			  return value;
		  } else {
			  // Return the retrieved value
			  return value;
		  }
	  }
  }

  virtual void remove(const Arg1& k, const Arg2& k2) {
	  key_type hash = _fn->hash(k, k2);

	  remove(hash);
  }

  virtual void remove(const key_type& hash) {
	  Locker writeLock(&_key_to_value);

	  if (!_key_to_value.containsKey(hash))
		  return;

	  map_value_type entry = _key_to_value.remove(hash);

	  const key_traker_iterator_type it = entry.second();

	  _key_tracker.erase(it);
  }

  int getHitCount() const {
	  return hitCount.get();
  }

  int getMissCount() const {
	  return missCount.get();
  }

  ReadWriteLock* getLock() {
	  return &_key_to_value;
  }

  int getSize() const {
	  return _key_to_value.size();
  }

protected:
  // Record a fresh key-value pair in the cache
  virtual bool insert(const Arg1& k, const Arg2& k2, const value_type& v) {
	  // Method is only called on cache misses
	  //assert(_key_to_value.find(k) == _key_to_value.end());
	  key_type hash = _fn->hash(k, k2);

	  Locker locker(&_key_to_value);

	  if (_key_to_value.containsKey(hash))
		  return false;

	  // Make space if necessary
	  if (_key_to_value.size() >= _capacity)
		  evict();

	  // Record k as most-recently-used key
	  key_traker_iterator_type it = _key_tracker.insert(_key_tracker.end(), hash);

	  // Create the key-value entry,
	  // linked to the usage record.
	  _key_to_value.put(hash, custom_make_pair(v, it));
	  // No need to check return,
	  // given previous assert.

	  return true;
  }

  // Purge the least-recently-used element in the cache
  virtual value_type evict() {
	  // Assert method is never called when cache is empty
	  E3_ASSERT(!_key_tracker.empty());

	  // Identify least recently used key
	  // Erase both elements to completely purge record
	  map_value_type ret = _key_to_value.remove(_key_tracker.front());
	  _key_tracker.pop_front();

	  return ret.first();
  }

protected:
  // The function to be cached
  //value_type (*_fn)(const key_type&);
  function_type _fn;

  // Maximum number of key-value pairs to be retained
  const size_t _capacity;

  // Key access history
  key_tracker_type _key_tracker;

  // Key-to-value lookup
  key_to_value_type _key_to_value;

  const int minAccessCountForPromoting;

  AtomicInteger hitCount, missCount;
};

#endif
