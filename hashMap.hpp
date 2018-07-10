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
			mTable[i] = nullptr;
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
			while (temp != nullptr) {
				next = temp->getNext();
				delete temp;
				temp = next;
			}
		}
		delete[] mTable;
	}

	/*
	 * Returns a pointer to the value of the link with the given key. Returns nullptr if no 
	 * link with the given key is in the table.
	 * @param key 
	 * @return link value or nullptr 
	 */
	V* mapGet(const K &key) {
		// get index of bucket
		int index = HASH_FUNCTION(key) % mapCapacity();
		if (index < 0)
			index += mapCapacity();

		HashLink<K, V> *temp = mTable[index];
		while (temp != nullptr) {
			if (temp->getKey().compare(key) == 0) {
				V tempVal = temp->getValue();
				return &tempVal;
			}

			temp = temp->getNext();
		}
		return nullptr;
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
				while (temp->getNext() != nullptr) 
					temp = temp->getNext();

				temp->setNext(newLink);
			}
			mSize++;
		}
	}

	/*
	 * Attemps to remove the key-value pair specified by the key parameter from the hash table.
	 * Returns true if removal was successful and false otherwise
	 * @param key 
	 * @return bool indicating whether key-value pair removal was successful
	 */
	bool mapRemove(const K &key) {
		if (mapContains(key)) {
			int index = HASH_FUNCTION(key) % mapCapacity();
			if (index < 0)
				index += mCapacity;

			HashLink<K, V> *temp = mTable[index];
			HashLink<K, V> *prev = nullptr;
			while (temp != nullptr) {
				if (temp->getKey().compare(key) == 0) {
					if (prev)
						prev->setNext(temp->getNext());

					mTable[index] = temp->getNext();
					delete temp;
					mSize--;
					return true;
				}

				else {
					prev = temp;
					temp = temp->getNext();
				}
			}
		}

		return false;
	}

	/*
	 * Attemps to locate the key-value pair specified by the key parameter from the hash table.
	 * Returns true if the key-value pair is found in the table and false otherwise
	 * @param key 
	 * @return bool indicating whether key exists in table
	 */
	bool mapContains(const K &key) {
		int index = HASH_FUNCTION(key) % mapCapacity();
		if (index < 0)
			index += mCapacity;

		HashLink<K, V> *temp = mTable[index];
		while (temp != nullptr) {
			if (temp->getKey().compare(key) == 0)
				return true;

			temp = temp->getNext();
		}

		return false;
	}

	/*
	 * Returns the number of links in the hash table
	 * @return number of links
	 */
	int mapSize() const { return mSize; }

	/*
	 * Returns number of buckets in the hash table
	 * @return number of buckets
	 */
	int mapCapacity() const { return mCapacity; } 

	/*
	 * Returns a pointer to the bucket in the table specified by the index
	 * @returns HashLink pointer to specified bucket index
	 */
	HashLink<K, V>* mapTableLink(int index) const { return mTable[index]; }

	/*
	 * Returns the number of table buckets without any links
	 * @return numbeer of empty buckets
	 */
	int mapEmptyBuckets() const {
		int empty = 0;
		for (int i = 0; i < mapCapacity(); i++) {
			if (mTable[i] == nullptr)
				empty++;
		}

		return empty;
	}

	/*
	 * Returns the ratio of (links / buckets) in the table currently
	 * @return map table load
	 */
	double mapTableLoad() const {
		double links = (double)mapSize();
		double buckets = (double)mapCapacity();
		return (links / buckets);
	}

	/*
	 * Removes all links in the table and frees allocated memory. Used as a helper function
	 * in the resizeTable() method when the table load exceeds MAP_TABLE_LOAD threshold
	 * @param hash table to be cleaned
	 * @param capacity of table
	 */
	void hashMapCleanup(HashLink<K, V> **map, int capacity) {
		HashLink<K, V> *temp, *next;

		// free link pointers in each bucket
		for (int i = 0; i < capacity; i++) {
			temp = map[i];
			while (temp != nullptr) {
				next = temp->getNext();
				delete temp;
				temp = next;
			}
		}
	}

	/*
	 * Resizes the hash table to contain newCapacity number of buckets. After the new
	 * table is allocated, all old links are re-hashed into the new table. The old table
	 * memory is then deallocated, including all of the old links
	 * @param new capacity (number of buckets)
	 */
	void resizeTable(int newCapacity) {
		// keep reference to old table for copy and delete
		HashLink<K, V> **oldTable = mTable;
		int oldCapacity = mCapacity;
		mCapacity = newCapacity;

		// allocate new hash map with updated capacity
		mTable = new HashLink<K, V> *[newCapacity]();

		for (int i = 0; i < newCapacity; i++)		
			mTable[i] = nullptr;

		// reset size counter and update new size with mapPut()
		mSize = 0;
		for (int i = 0; i < oldCapacity; i++) {
			HashLink<K, V> *temp = oldTable[i];

			// rehash links from old table into new table
			while (temp != nullptr) {
				K tempKey = temp->getKey();
				V tempVal = temp->getValue();
				mapPut(tempKey, tempVal);
				temp = temp->getNext();
			}
		}

		// delete links from old table, set new table capacity, and free the old table
		hashMapCleanup(oldTable, oldCapacity);
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
			if (entry != nullptr) {
				os << "Bucket " << i << " -> ";
				while (entry != nullptr) {
					os << "(" << entry->getKey() << ", " << entry->getValue() << ") -> ";
					entry = entry->getNext();
				}
				os << "nullptr";
				os << endl;
			}
		}
		os << endl;
		return os;
	}

	/*
	 * Hashes the key by folding (summing) each character
	 * @returns hashed value for input key
	 */
	int hashFunction1(const K &key) {
		int r = 0;
		for (int i = 0; key[i] != '\0'; i++)
			r += key[i];

		return r;
	}

	/*
	 * Hashes the key by shifting the value of each character, then folding (summing)
	 * hashFunction2 prevents anagrams hashing to the same value (via the shift), thus
	 * resulting in fewer collisions
	 * @returns hashed value for input key
	 */
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
