#include "iterator.h"

// Create a new iterator
Iterator create_iterator(void *array, int size, int element_size) {
  Iterator it;
  it.array = array;
  it.size = size;
  it.element_size = element_size;
  it.index = 0; // Start at the beginning
  return it;
}

// Check if there are more elements to iterate over
inline bool has_next(Iterator it) { return it.index < it.size; }

// Get the current element without advancing the iterator
void *current(Iterator *it) {
  if (it->index < it->size) {
    // Calculate the address of the current element
    return (char *)it->array + (it->index * it->element_size);
  }
  return NULL; // Return NULL if out of bounds
}

// Move to the next element and return it
void *next(Iterator *it) {
  if (has_next(*it)) {
    // Return the current element and advance the index
    void *current_element = current(it);
    it->index++; // Move to the next element
    return current_element;
  }
  return NULL; // No more elements
}
