/*
Copyright (C) 2007 <SWGEmu>. All rights reserved.
Distribution of this file for usage outside of Core3 is prohibited.
*/

#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include "../lang.h"

namespace sys {
  namespace util {
		
	template<class K, class V> class HashTable;
	template<class K, class V> class HashTableIterator;
	
	template<class K, class V> class Entry {
		int hash;
		K key;
		V value;
		Entry* next;
	
	public:
		Entry() {
			next = NULL;
		}
	
		Entry(int hash, K key, V value, Entry* e) {
			Entry::hash = hash;
			Entry::key = key;
			Entry::value = value;
			next = e;
		}
		
		friend class HashTable<K,V>;
		friend class HashTableIterator<K,V>;
	};
	
	template<class K, class V> class HashTable {
	    Entry<K,V>** table;
		int tableLength;    
	    
	    int count;
		
	    int initialCapacity;
	    int threshold;
	    float loadFactor;
	    
	    int modCount;
	
	    virtual int hash(const K& key) = 0;
	
	protected:
		V nullValue;
	
	public:
	    HashTable();
		HashTable(int initialCapacity);
	    HashTable(int initialCapacity, float loadFactor);
	
		virtual ~HashTable();
	    
	    V& put(const K& key, const V& value);
	        
	    V& get(const K& key);
	    
	    bool containsKey(const K& key);
	
	    V remove(const K& key);

	    void removeAll();
	    
	protected:
	    void init(int initialCapacity, float loadFactor);
	        
	   	void rehash();
	
	public:
	 	inline int size() {
			return count;
	    }
	    
	    inline bool isEmpty() {
			return count == 0;
	    }
	
	    inline void setNullValue(V val) {
	    	nullValue = val;	
	    }
	    
	    friend class HashTableIterator<K,V>;
	    
	};
	
	template<class K, class V> class HashTableIterator {
		HashTable<K,V>* htable;
		Entry<K,V>* e;
		
		int position;
		int eIndex;
			
	public:
		HashTableIterator();
		HashTableIterator(HashTable<K,V>* Table);
		
		V& getNextValue();
		
		K& getNextKey();
		
		void getNextKeyAndValue(K& key, V& value);

		V& next();
		
		bool hasNext();
		
		void resetIterator();
		
		void toConsole();

		inline void setTable(HashTable<K,V>* table) {
			htable = table;
		}
	};
	
	template<class K, class V> HashTable<K,V>::HashTable(int initialCapacity, float loadFactor) {
		init(initialCapacity, loadFactor);
	}
	    
	template<class K, class V> HashTable<K,V>::HashTable(int initialCapacity) {
		init(initialCapacity, 0.75f);
	}
	
	template<class K, class V> HashTable<K,V>::HashTable() {
		init(11, 0.75f);
	}
	
	template<class K, class V> HashTable<K,V>::~HashTable() {
		removeAll();
		
		free(table);
	}
	
	template<class K, class V> void HashTable<K,V>::init(int initialCapacity, float loadFactor) {
		if (initialCapacity == 0)
			HashTable::initialCapacity = 1;
		
		table = (Entry<K, V>**) malloc(initialCapacity * sizeof(Entry<K, V>*));
		memset(table, 0, initialCapacity * sizeof(Entry<K, V>*));
		tableLength = initialCapacity;
			
		HashTable::loadFactor = loadFactor;
		threshold = (int) (initialCapacity * loadFactor);
	
	   	count = 0;
	   	modCount = 0;
	}
	    
	template<class K, class V> void HashTable<K,V>::rehash() {
		int oldCapacity = tableLength;
		Entry<K,V>** oldMap = table;
	
		int newCapacity = oldCapacity * 2 + 1;
			
		Entry<K,V>** newMap = (Entry<K, V>**) malloc(newCapacity * sizeof(Entry<K, V>*));
		memset(newMap, 0, newCapacity * sizeof(Entry<K, V>*));
	
		modCount++;
		threshold = (int) (newCapacity * loadFactor);
		table = newMap;
	
		for (int i = oldCapacity; i-- > 0 ;) {
	    	for (Entry<K,V>* old = oldMap[i]; old != NULL ;) {
				Entry<K,V>* e = old;
				old = old->next;
	
				int index = (e->hash & 0x7FFFFFFF) % newCapacity;
				e->next = newMap[index];
				newMap[index] = e;
	    	}
		}
		
		tableLength = newCapacity;
			
		free(oldMap);
	}
	    
	template<class K, class V> V& HashTable<K,V>::put(const K& key, const V& value) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;
			
		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				V& old = e->value;
				e->value = value;
				return old;
	    	}
		}
	
		modCount++;
		if (count >= threshold) {
	    	rehash();
	
			index = (hashCode & 0x7FFFFFFF) % tableLength;
		} 
	
		Entry<K,V>* e = table[index];
		table[index] = new Entry<K,V>(hashCode, key, value, e);
		count++;
			
		return nullValue;
	}
	    
	template<class K, class V> V& HashTable<K,V>::get(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;
	
		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				return e->value;
	    	}
		}
			
		return nullValue;
	}
	    
	template<class K, class V> bool HashTable<K,V>::containsKey(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;
			
		for (Entry<K,V>* e = table[index]; e != NULL; e = e->next) {
		    if ((e->hash == hashCode) && e->key == key) {
				return true;
	    	}
		}
			
		return false;
	}
	
	template<class K, class V> V HashTable<K,V>::remove(const K& key) {
		int hashCode = hash(key);
		int index = (hashCode & 0x7FFFFFFF) % tableLength;
	
		for (Entry<K,V> *e = table[index], *prev = NULL; e != NULL; prev = e, e = e->next) {
	    	if ((e->hash == hashCode) && e->key == key) {
				if (prev != NULL)
			    	prev->next = e->next;
				else
		    		table[index] = e->next;
					
				count--;
				V oldValue = e->value;
				e->value = nullValue;
				
				delete e;
				
				return oldValue;
	    	}
		}
			
		return nullValue;
	}

	template<class K, class V> void HashTable<K,V>::removeAll() {
		if (count == 0)
			return;
		
		for (int i = 0; i < tableLength; ++i) {
			for (Entry<K,V>* e = table[i]; e != NULL;) {
				Entry<K,V>* next = e->next;
				
				delete e;
				e = next;
			}

			table[i] = NULL;
		}
		
		count = 0;
	}

	template<class K, class V> HashTableIterator<K,V>::HashTableIterator() {
		htable = NULL;
		
		position = 0;
		eIndex = -1;
		
		e = NULL;
	}

	template<class K, class V> HashTableIterator<K,V>::HashTableIterator(HashTable<K,V>* Table) {
		htable = Table;
		
		position = 0;
		eIndex = -1;
		
		e = NULL;
	}
	
	template<class K, class V> V& HashTableIterator<K,V>::getNextValue() {
		while (e == NULL)
			e = htable->table[++eIndex];
		
		V& result = e->value;
		
		e = e->next;
		position++;

		return result;	
	}
	
	template<class K, class V> K& HashTableIterator<K,V>::getNextKey() {
		while (e == NULL)
			e = htable->table[++eIndex];
		
		K& result = e->key;
		
		e = e->next;
		position++;

		return result;	
	}
	
	template<class K, class V> void HashTableIterator<K,V>::getNextKeyAndValue(K& key, V& value) {
		while (e == NULL)
			e = htable->table[++eIndex];

		key = e->key;
		value = e->value;
		
		e = e->next;
		position++;
	}

	template<class K, class V> V& HashTableIterator<K,V>::next() {
		return getNextValue();
	}
	
	template<class K, class V> bool HashTableIterator<K,V>::hasNext() {
	 	return position < htable->size();
	}
	
	template<class K, class V> void HashTableIterator<K,V>::resetIterator() {
		position = 0;
		eIndex = -1;
		
		e = NULL;
	}
	
	template<class K, class V> void HashTableIterator<K,V>::toConsole() {
		while(hasNext()) {
			while (e == NULL)
				e = htable->table[++eIndex];

			cout << e->key << " = " << e->value << "\n";
		
			e = e->next;
			position++;
		}
	}

  } // namespace util
} // namespace sys

using namespace sys::util;

#endif /*HASHTABLE_H_*/
