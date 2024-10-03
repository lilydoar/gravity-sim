#ifndef OPTIONAL_H
#define OPTIONAL_H

#include <stdbool.h>
#include <stdlib.h>

// Define the optional struct for a generic type
#define DEFINE_OPTIONAL(type)                                                  \
  typedef struct {                                                             \
    bool is_some;                                                              \
    type value;                                                                \
  } Optional_##type;                                                           \
                                                                               \
  /* Function to create an optional with a value */                            \
  Optional_##type some_##type(type value) {                                    \
    Optional_##type opt;                                                       \
    opt.is_some = true;                                                        \
    opt.value = value;                                                         \
    return opt;                                                                \
  }                                                                            \
                                                                               \
  /* Function to create an empty optional (none) */                            \
  Optional_##type none_##type(void) {                                          \
    Optional_##type opt;                                                       \
    opt.is_some = false;                                                       \
    return opt;                                                                \
  }                                                                            \
                                                                               \
  /* Function to check if the optional has a value */                          \
  bool is_some_##type(Optional_##type opt) { return opt.is_some; }             \
                                                                               \
  /* Function to unwrap the value (assumes value is present) */                \
  type unwrap_##type(Optional_##type opt) {                                    \
    if (!opt.is_some) {                                                        \
      exit(EXIT_FAILURE); /* Crash if unwrapping a none */                     \
    }                                                                          \
    return opt.value;                                                          \
  }

#endif // OPTIONAL_H
