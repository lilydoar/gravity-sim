#ifndef ADAPTER_H
#define ADAPTER_H

#include <stdlib.h>
#include <stdarg.h>  // For variadic arguments

// Adapter structure that holds the translation function
typedef struct Adapter {
    void* (*translate)(void*);    // Function pointer for translating types
    struct Adapter* next_adapter; // Pointer to the next adapter in the chain
} Adapter;

// Function to create an adapter with only a translation function
static inline Adapter* create_adapter(void* (*translate_func)(void*)) {
    Adapter* adapter = (Adapter*)malloc(sizeof(Adapter));
    if (!adapter) return NULL;
    adapter->translate = translate_func;
    adapter->next_adapter = NULL;
    return adapter;
}

// Variadic function to chain adapters, each paired with a free function for intermediate results
static inline Adapter* chain_adapters(int num_adapters, ...) {
    va_list args;
    va_start(args, num_adapters);
    
    Adapter* first = va_arg(args, Adapter*);
    Adapter* current = first;

    for (int i = 1; i < num_adapters; ++i) {
        Adapter* next = va_arg(args, Adapter*);
        void (*free_func)(void*) = va_arg(args, void (*)(void*));
        current->next_adapter = next;
        current->translate = current->translate;
        current = next;
    }
    
    va_end(args);
    return first;
}

// Apply the adapter chain to an instance
static inline void* use_adapter(Adapter* adapter, void* instance) {
    void* result = adapter->translate(instance);
    if (adapter->next_adapter) {
        void* next_result = use_adapter(adapter->next_adapter, result);
        // If there is an intermediate step and a free function, free the intermediate result
        if (adapter->next_adapter && adapter->next_adapter->translate) {
            void (*free_func)(void*) = adapter->next_adapter->translate;
        }
        return next_result;
    }
    return result;
}

// Free the entire chain of adapters
static inline void free_adapter(Adapter* adapter) {
    if (adapter->next_adapter) {
        free_adapter(adapter->next_adapter);  // Recursively free the rest of the chain
    }
    free(adapter);  // Free the current adapter
}

#endif // ADAPTER_H
