#ifndef TB_HASHMAP_H
#define TB_HASHMAP_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct _tb_hashmap_iter tb_hashmap_iter;

#define TB_HASHMAP_INSERT(map, key, value) 

typedef enum
{
    TB_HASHMAP_OK = 0,
    TB_HASHMAP_ERROR,
    TB_HASHMAP_ALLOC_ERROR,
    TB_HASHMAP_HASH_ERROR,
    TB_HASHMAP_KEY_NOT_FOUND
} tb_hashmap_error;

typedef struct
{
    void* key;
    void* value;
} tb_hashmap_entry;


typedef void* (*tb_hashmap_alloc)(void* allocator, size_t count, size_t size);
typedef void  (*tb_hashmap_free) (void* allocator, void* block);

typedef int  (*tb_hashmap_entry_alloc)(void* allocator, tb_hashmap_entry* entry, void* key, void* value);
typedef void (*tb_hashmap_entry_free) (void* allocator, tb_hashmap_entry* entry);

typedef struct
{
    tb_hashmap_entry* table;
    size_t capacity;
    size_t used;
    size_t  (*hash)(const void*);
    int     (*key_cmp)(const void*, const void*);

    /* memory */
    void* allocator;

    tb_hashmap_alloc alloc;
    tb_hashmap_free  free;

    tb_hashmap_entry_alloc entry_alloc;
    tb_hashmap_entry_free  entry_free;
} tb_hashmap;

/*
 * Initialize an empty hashmap.
 *
 * hash_func should return an even distribution of numbers between 0 and SIZE_MAX varying on the key provided.
 *
 * key_compare_func should return 0 if the keys match, and non-zero otherwise.
 *
 * initial_size is optional, and may be set to the max number of entries expected to be put in the hash table.
 * This is used as a hint to pre-allocate the hash table to the minimum size needed to avoid gratuitous rehashes. 
 * If initial_size is 0, a default size will be used.
 *
 * Returns TB_HASHMAP_OK on success and tb_hashmap_error on failure.
 */
tb_hashmap_error tb_hashmap_init(tb_hashmap* map, size_t(*hash)(const void*), int (*cmp)(const void*, const void*), size_t initial_capacity);

/*
 * Free the hashmap and all associated memory.
 */
void tb_hashmap_destroy(tb_hashmap* map);

tb_hashmap_error tb_hashmap_rehash(tb_hashmap* map, size_t new_capacity);

/*
 * Insert an entry to the hashmap.  
 * If an entry with a matching key already exists and has a value pointer associated with it, NULL is returned, 
 * instead of assigning the new value.
 * Returns NULL if memory allocation failed.
 */
void* tb_hashmap_insert(tb_hashmap* map, const void* key, void* value);

/*
 * Remove an entry with the specified key from the map.
 * Returns TB_HASHMAP_KEY_NOT_FOUND if no entry was found, else TB_HASHMAP_OK.
 */
tb_hashmap_error tb_hashmap_remove(tb_hashmap* map, const void *key);

/*
 * Remove all entries.
 */
void tb_hashmap_clear(tb_hashmap* map);

/*
 * Return the value pointer, or NULL if no entry was found.
 */
void* tb_hashmap_find(const tb_hashmap* map, const void* key);

/*
 * Return the key pointer, or NULL if no entry was found.
 */
const void* tb_hashmap_get_key_ptr(const tb_hashmap* map, const void* key);

/*
 * Find the hashmap entry with the specified key, or an empty slot.
 * Returns NULL if the entire table has been searched without finding a match.
 */
tb_hashmap_entry* tb_hashmap_entry_find(const tb_hashmap* map, const void* key, int find_empty);


/*
 * Removes the specified entry and processes the proceeding entries to reduce the load factor and keep the 
 * chain continuous. This is a required step for hash maps using linear probing.
 */
void tb_hashmap_entry_remove(tb_hashmap* map, tb_hashmap_entry* removed_entry);

/*
 * Get a new hashmap iterator.
 * The iterator is an opaque pointer that may be used with hashmap_iter_*() functions.
 * Hashmap iterators are INVALID after a remove operation is performed.
 * hashmap_iter_remove() allows safe removal during iteration.
 */
tb_hashmap_iter* tb_hashmap_iterator(const tb_hashmap* map);

/*
 * Return an iterator to the next hashmap entry.
 * Returns NULL if there are no more entries.
 */
tb_hashmap_iter* tb_hashmap_iter_next(const tb_hashmap* map, const tb_hashmap_iter* iter);

/*
 * Remove the hashmap entry pointed to by this iterator and returns an iterator to the next entry.
 * Returns NULL if there are no more entries.
 */
tb_hashmap_iter* tb_hashmap_iter_remove(tb_hashmap* map, const tb_hashmap_iter* iter);

/*
 * Return the key of the entry pointed to by the iterator.
 */
const void* tb_hashmap_iter_get_key(const tb_hashmap_iter* iter);

/*
 * Return the value of the entry pointed to by the iterator.
 */
void* tb_hashmap_iter_get_value(const tb_hashmap_iter* iter);

/* 
 * Hash functions
 */
size_t tb_hash_string(const char* str);

uint32_t tb_hash_uint32(uint32_t i);
uint64_t tb_hash_uint64(uint64_t i);

#endif /* !TB_HASHMAP_H */
