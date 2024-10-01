#ifndef ITERATOR_H
#define ITERATOR_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// Iterator structure definition
typedef struct {
    void* array;       // Generic array (pointer to any data type)
    int element_size;  // Size of each element in the array (in bytes)
    int size;          // Number of elements in the array
    int index;         // Current position in the array
} Iterator;

// Function to create a new iterator for any array
Iterator create_iterator(void* array, int size, int element_size);

// Function to check if there are more elements
bool has_next(Iterator* it);

// Function to get the current element
void* current(Iterator* it);

// Function to move to the next element and return it
void* next(Iterator* it);

// Macro to simplify iteration over the iterator
#define ITERATE(it, type, value_ptr) \
    for (type* value_ptr = next(&it); value_ptr != NULL; value_ptr = next(&it))

// Implementation section
#ifdef ITERATOR_IMPLEMENTATION

// Create a new iterator
Iterator create_iterator(void* array, int size, int element_size) {
    Iterator it;
    it.array = array;
    it.size = size;
    it.element_size = element_size;
    it.index = 0;  // Start at the beginning
    return it;
}

// Check if there are more elements to iterate over
bool has_next(Iterator* it) {
    return it->index < it->size;
}

// Get the current element without advancing the iterator
void* current(Iterator* it) {
    if (it->index < it->size) {
        // Calculate the address of the current element
        return (char*)it->array + (it->index * it->element_size);
    }
    return NULL;  // Return NULL if out of bounds
}

// Move to the next element and return it
void* next(Iterator* it) {
    if (has_next(it)) {
        // Return the current element and advance the index
        void* current_element = current(it);
        it->index++;  // Move to the next element
        return current_element;
    }
    return NULL;  // No more elements
}

#endif // ITERATOR_IMPLEMENTATION

#endif // ITERATOR_H
