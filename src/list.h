#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

struct listitem {
	void *value;
	struct listitem *next;
};

/**
 * Prepends an item to the given head.
 *
 * @param head
 * @param data
 * @return the new head
 */
struct listitem *list_prepend(struct listitem *, void *);

/**
 * Appends an item to the given head.
 *
 * @param head
 * @param data
 * @return the inserted listitem
 */
struct listitem *list_append(struct listitem *head, void *value);

/**
 * Linearly searches a list for the matching item
 *
 * @param head
 * @param first parameter to the equality function
 * @param equality function
 * @param if not NULL contains the index of the found data
 * @return NULL if not found, otherwise the pointer to the first match
 */
void  *list_find(struct listitem *, void *, bool(*)(void *, void *), uint32_t *);

/**
 * Delete an item from the list
 *
 * @param head
 * @param index
 * @param pointer to where to put the pointer to the deleted data
 * @return new head
 */
struct listitem *list_delete(struct listitem *, uint32_t, void **);

/**
 * Calculates the length of the list
 *
 * @param head
 * @return length
 */
size_t list_length(struct listitem *);

/**
 * Apply a function on all elements of the list
 *
 * @param head
 * @param initial state
 * @param state function
 */
void  *list_iterate(struct listitem *, void *, void *(*)(void *st, void *el));

/**
 * Free the list and apply a free function on the elements
 *
 * @param head
 * @param free function
 */
void   list_free(struct listitem *, void(*)(void *));

/**
 * No-op free function to be used in list_free
 *
 * @param data to free
 */
void   list_free_ignore(void *);
void  *list_to_array(struct listitem *l, uint64_t *len);

#endif
