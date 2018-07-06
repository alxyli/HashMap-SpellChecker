/*
 * Name: Alex Li
 * hashLink header
 */

#pragma once

#ifndef NULL
#define NULL 0
#endif

template <typename K, typename V>
class HashLink {
public:
	HashLink(const K &key, const V &value) : mKey(key), mValue(value), next(NULL) {};
	K getKey() const { return mKey; };
	V getValue() const { return mValue; };
	HashLink* getNext() const { return next; };
	void setNext(HashLink* n) { next = n; };
	void setValue(V value) { mValue = value; };

private:
	K mKey;
	V mValue;
	HashLink *next;
};

