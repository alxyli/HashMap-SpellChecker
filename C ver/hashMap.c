/*
 * Name: Alex Li
 * Date: 3/8/17
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++)
    {
        map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    assert (map != NULL);

    HashLink *temp, *next;

    // iterate through table, freeing temp until all links in the map have been freed
    for (int i = 0; i < map->capacity; i++)
    {
        temp = map->table[i];
        while (temp != NULL)
        {
            next = temp->next;
            hashLinkDelete(temp);
            temp = next;
        }
    }

    // then free the table
    free(map->table);
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    assert (map != NULL && key != NULL);

    // find index of correct linked list bucket
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
        index += hashMapCapacity(map);

    // set temp to this bucket
    HashLink *temp = map->table[index];

    while (temp != NULL)
    {
        // found a match, return a pointer to the value of the link with the given key
        if (strcmp(temp->key, key) == 0)
            return &(temp->value);

        // if no match, move temp to next link and check again until next is null
        temp = temp->next;
    }

    // no matching value with given key, return NULL
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given
 * capacity. After allocating the new table, all of the links need to be
 * rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    assert (map != NULL && capacity > 0);

    // create a new hash map with new capacity
    HashMap *newMap = hashMapNew(capacity);
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink *temp = map->table[i];

        // rehash links from old table into new table
        while (temp != NULL)
        {
            hashMapPut(newMap, temp->key, temp->value);
            temp = temp->next;
        }
    }

    // clean up old map, set new hash map attributes, free the newMap
    hashMapCleanUp(map);
    map->table = newMap->table;
    map->size = newMap->size;
    map->capacity = newMap->capacity;

    free(newMap);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // resize table if table load exceeds max (.75 default)
    if (hashMapTableLoad(map) >= MAX_TABLE_LOAD)
        resizeTable(map, hashMapCapacity(map) * 2);

    // get correct bucket index
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
        index += hashMapCapacity(map);

    // a link with the given key already exists, update its value
    if (hashMapContainsKey(map, key))
    {
        // valPtr points to the value of the link with the given key
        int *valPtr = hashMapGet(map, key);
        // update this link's value with the given value
        *valPtr = value;
    }

    // create a new link and add it to the bucket
    else
    {   
        // add new link to end of list
        HashLink *newLink = hashLinkNew(key, value, NULL);
        // bucket is currently empty
        if (!map->table[index])
            map->table[index] = newLink;
        
        else
        {
            // iterate temp to last element and add the new link
            HashLink *temp = map->table[index];
            while (temp->next != NULL)
                temp = temp->next;
            
            temp->next = newLink;
        }
        map->size++;
    }
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    assert (map != NULL && key != NULL);

    // get correct bucket index
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
        index += hashMapCapacity(map);

    if (hashMapContainsKey(map, key))
    {
        HashLink *temp = map->table[index];
        HashLink *prev = NULL;
        while (temp != NULL)
        {
            // remove link with given key from map and adjust pointers
            if (strcmp(temp->key, key) == 0)
            {
                if (prev)
                    prev->next = temp->next;

                map->table[index] = temp->next;
                hashLinkDelete(temp);
                map->size--;
                temp = NULL;
            }

            else
            {
                prev = temp;
                temp = temp->next;
            }
        }
    }

    // does nothing if link with given key doesn't exist
    else
        return;
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    assert (map != NULL && key != NULL);

    // get correct bucket index
    int index = HASH_FUNCTION(key) % hashMapCapacity(map);
    if (index < 0)
        index += hashMapCapacity(map);

    HashLink *temp = map->table[index];
    while (temp != NULL)
    {
        //  return 1 if given key found in table
        if (strcmp(temp->key, key) == 0)
            return 1;

        // move to next link and check
        temp = temp->next; 
    }

    // key not in table
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    assert (map != NULL);
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    assert (map != NULL);
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    int empty = 0;

    for (int i = 0; i < hashMapCapacity(map); i++)
    {
        if (map->table[i] == NULL)
            empty++;
    }

    return empty;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    float links = (float)hashMapSize(map);
    float buckets = (float)hashMapCapacity(map);

    return (links / buckets);
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    for (int i = 0; i < map->capacity; i++)
    {
        HashLink* link = map->table[i];
        if (link != NULL)
        {
            printf("\nBucket %i ->", i);
            while (link != NULL)
            {
                printf(" (%s, %d) ->", link->key, link->value);
                link = link->next;
            }
        }
    }
    printf("\n");
}