/*
 * Alex Li
 * hashMap header
 */

#pragma once
#include "hashLink.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

#define HASH_FUNCTION hashFunction1
#define MAX_TABLE_LOAD .75

template <typename K, typename V>
class HashMap {
public:
	/*
	 * Parameterized HashMap constructor
	 * @param capacity
	 */
	HashMap(int capacity) {
		mCapacity = capacity;
		mSize = 0;
		mTable = new HashLink<K, V> *[capacity]();
		for (int i = 0; i < capacity; i++)		
			mTable[i] = NULL;
	}

	/*
	 * HashMap destructor
	 * Frees link pointers in buckets and delete table
	 */
	~HashMap() { 
		HashLink<K, V> *temp, *next;

		// free all links in each bucket
		for (int i = 0; i < mCapacity; i++) {
			temp = mTable[i];
			while (temp != NULL) {
				next = temp->getNext();
				delete temp;
				temp = next;
			}
		}
		delete[] mTable;
	}

	/*
	 * Returns a pointer to the value of the link with the given key. Returns NULL if no 
	 * link with the given key is in the table.
	 * @param key 
	 * @return link value or NULL 
	 */
	V* mapGet(const K &key) {
		// get index of bucket
		int index = HASH_FUNCTION(key) % mapCapacity();
		if (index < 0)
			index += mapCapacity();

		HashLink<K, V> *temp = mTable[index];
		while (temp != NULL) {
			if (temp->getKey().compare(key) == 0) {
				V tempVal = temp->getValue();
				return &tempVal;
			}

			temp = temp->getNext();
		}
		return NULL;
	}

	/*
	 * Updates the given key-value pair in the hash table if a link with the given key
	 * already exists. Otherwise allocates a new link with the given key-value pair and adds it
	 * to the corresponding bucket's linked list.
	 * @param key 
	 * @param value 
	 */
	void mapPut(const K key, const V value) {
		// resize table if table load exceeds max threshold (default .75)
		if (mapTableLoad() >= MAX_TABLE_LOAD)
			resizeTable(mCapacity * 2);

		// get index of bucket
		int index = HASH_FUNCTION(key) % mapCapacity();
		if (index < 0)
			index += mCapacity;

		// update link value if key exists in table
		if (mapContains(key)) {
			HashLink<K, V> *entry = mTable[index];
			entry->setValue(value);
		}

		// otherwise add new link to bucket
		else {
			HashLink<K, V> *newLink = new HashLink<K, V> (key, value);

			if (!mTable[index]) 
				mTable[index] = newLink;

			else {
				// add new link to end of bucket
				HashLink<K, V> *temp = mTable[index];
				while (temp->getNext() != NULL) 
					temp = temp->getNext();

				temp->setNext(newLink);
			}
			mSize++;
		}
	}

	void mapRemove(const K &key);

	bool mapContains(const K &key) {
		int index = HASH_FUNCTION(key) % mapCapacity();
		if (index < 0)
			index += mCapacity;

		HashLink<K, V> *temp = mTable[index];
		while (temp != NULL) {
			if (temp->getKey().compare(key) == 0)
				return true;

			temp = temp->getNext();
		}

		return false;
	}

	int mapSize() const { return mSize; }
	int mapCapacity() const { return mCapacity; } 
	int mapEmptyBuckets() const {
		int empty = 0;
		for (int i = 0; i < mapCapacity(); i++) {
			if (mTable[i] == NULL)
				empty++;
		}

		return empty;
	}

	double mapTableLoad() const {
		double links = (double)mapSize();
		double buckets = (double)mapCapacity();
		return (links / buckets);
	}

	void hashMapCleanup(HashLink<K, V> **map) {
		HashLink<K, V> *temp, *next;

		// free link pointers in each bucket
		for (int i = 0; i < mCapacity; i++) {
			temp = map[i];
			while (temp != NULL) {
				next = temp->getNext();
				delete temp;
				temp = next;
			}
		}
	}

	void resizeTable(int newCapacity) {
		// keep reference to old table for copy and delete
		HashLink<K, V> **oldTable = mTable;

		// allocate new hash map with updated capacity
		mTable = new HashLink<K, V> *[newCapacity]();

		for (int i = 0; i < newCapacity; i++)		
			mTable[i] = NULL;

		// reset size counter and update new size with mapPut()
		mSize = 0;
		for (int i = 0; i < mCapacity; i++) {
			HashLink<K, V> *temp = oldTable[i];

			// rehash links from old table into new table
			while (temp != NULL) {
				K tempKey = temp->getKey();
				V tempVal = temp->getValue();
				mapPut(tempKey, tempVal);
				temp = temp->getNext();
			}
		}

		// delete links from old table, set new table capacity, and free the old table
		hashMapCleanup(oldTable);
		mCapacity = newCapacity;
		delete[] oldTable;
	}


	/*
	 * Overload operator << for HashMap print functionality
	 * Prints the buckets and links in the map in the format Bucket n -> (key, value)
	 * @param output stream os, HashMap map
	 * @returns output stream containing formatted HashMap contents
	 */
	friend std::ostream& operator<<(std::ostream& os, const HashMap<K, V>& map) {
		for (int i = 0; i < map.mapCapacity(); i++) {
			HashLink<K, V> *entry = map.mTable[i];
			if (entry != NULL) {
				os << "Bucket " << i << " -> ";
				while (entry != NULL) {
					os << "(" << entry->getKey() << ", " << entry->getValue() << ") -> ";
					entry = entry->getNext();
				}
				os << "null";
				os << endl;
			}
		}
		os << endl;
		return os;
	}

	int hashFunction1(const K &key) {
		int r = 0;
		for (int i = 0; key[i] != '\0'; i++)
			r += key[i];

		return r;
	}

	int hashFunction2(const K &key) {
		int r = 0;
		for (int i = 0; key[i] != '\0'; i++)
			r += (i + 1) * key[i];

		return r;
	}

private:
	HashLink<K, V>** mTable;
	int mSize; // number of links in the table
	int mCapacity; // number of buckets
};
