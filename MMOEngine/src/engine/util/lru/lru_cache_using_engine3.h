
#include "system/util/SynchronizedHashTable.h"
#include "system/thread/atomic/AtomicInteger.h"

#include <utility>
#include <cassert>
#include <list>

// Class providing fixed-size (by number of records)
// LRU-replacement cache of a function with signature
// V f(K).

template<typename A, typename B>
class LRUCacheEntry {
	A key;
	B value;
	AtomicInteger accessCount;
public:
	LRUCacheEntry() {

	}

	LRUCacheEntry(const A& a, const B& b) : key(a), value(b) {

	}

#ifdef CXX11_COMPILER
	LRUCacheEntry(LRUCacheEntry&& a) : key(std::move(a.key)),
			value(std::move(a.value)), accessCount(a.accessCount) {

	}

	LRUCacheEntry& operator=(LRUCacheEntry&& a) {
		if (this == &a)
			return *this;

		key = std::move(a.key);
		value = std::move(a.value);

		accessCount = a.accessCount;

		return *this;
	}
#endif

	A& first() {
		return key;
	}

	B& second() {
		return value;
	}

	bool incrementAccessCount(int max) {
		int old, newCount;
		bool result;

		do {
			result = false;
			old = accessCount.get();

			newCount = old + 1;

			if (newCount == max) {
				newCount = 0;

				result = true;
			}
		} while (!accessCount.compareAndSet(old, newCount));

		return result;
	}

	int getAccessCount() {
		return accessCount.get();
	}

	void clearAccessCount() {
		int old = 0;

		do {
			old = accessCount.get();
		} while (accessCount.compareAndSet(old, 0));
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

template <typename K, typename V>
class lru_cache_using_engine3 {
public:

  typedef K key_type;
  typedef V value_type;

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
  lru_cache_using_engine3(value_type (*f)(const key_type&), size_t c, int accessCountToPromote = 5)
    	:_fn(f), _capacity(c), minAccessCountForPromoting(accessCountToPromote) {
    assert(_capacity != 0);
    assert(minAccessCountForPromoting > 0);
  }

  void clear() {
	  Locker locker(&_key_to_value);

	  _key_to_value.clear();
	  _key_tracker.clear();
  }

  // Obtain value of the cached function for k
  value_type operator()(const key_type& k) {
	  // Attempt to find existing record
	  ReadLocker readLocker(&_key_to_value);

	  Entry<K, map_value_type>* entry = _key_to_value.getEntryUnsafe(k);

	  if (entry == NULL) {// We don't have it:
		  // Evaluate function and create new record
		  readLocker.release();

		  const value_type v = _fn(k);

		  insert(k,v);

		  // Return the freshly computed value
		  return v;
	  } else { // We do have it:
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

			  if (entry != NULL) {
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

private:
  // Record a fresh key-value pair in the cache
  void insert(const key_type& k,const value_type& v) {
	  Locker locker(&_key_to_value);

	  // Method is only called on cache misses
	  //assert(_key_to_value.find(k) == _key_to_value.end());

	  if (_key_to_value.containsKey(k))
		  return;

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
  }

  // Purge the least-recently-used element in the cache
  void evict() {
	  // Assert method is never called when cache is empty
	  assert(!_key_tracker.empty());

	  // Identify least recently used key
	  // Erase both elements to completely purge record
	  _key_to_value.remove(_key_tracker.front());
	  _key_tracker.pop_front();
  }

  // The function to be cached
  value_type (*_fn)(const key_type&);

  // Maximum number of key-value pairs to be retained
  const size_t _capacity;

  // Key access history
  key_tracker_type _key_tracker;

  // Key-to-value lookup
  key_to_value_type _key_to_value;

  const int minAccessCountForPromoting;
};
