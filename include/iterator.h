#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

// Iterator structure definition
typedef struct {
  void *array;         // Generic array (pointer to any data type)
  size_t element_size; // Size of each element in the array (in bytes)
  int size;            // Number of elements in the array
  int index;           // Current position in the array
} Iterator;

// Function to create a new iterator for any array
Iterator create_iterator(void *array, int size, int element_size);

// Function to check if there are more elements
bool has_next(Iterator it);

// Function to get the current element
void *current(Iterator *it);

// Function to move to the next element and return it
void *next(Iterator *it);

// Macro to simplify iteration over the iterator
#define ITERATE(it, type, value_ptr)                                           \
  for (value_ptr = next(&it); value_ptr != NULL; value_ptr = next(&it))

// Function declarations
Iterator create_iterator(void *array, int size, int element_size);
bool has_next(Iterator it);
void *current(Iterator *it);
void *next(Iterator *it);


#endif // ITERATOR_H
