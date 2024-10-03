#include "optional.h"
#include <assert.h>
#include <stdio.h>

// Define optionals for types to be tested
DEFINE_OPTIONAL(int)
DEFINE_OPTIONAL(float)
DEFINE_OPTIONAL(char)

// Test creating an optional with a value
void test_some_int() {
  Optional_int opt = some_int(10);
  assert(is_some_int(opt) == true);
  assert(unwrap_int(opt) == 10);
  printf("test_some_int passed\n");
}

// Test creating an optional with no value
void test_none_int() {
  Optional_int opt = none_int();
  assert(is_some_int(opt) == false);
  printf("test_none_int passed\n");
}

// Test creating and checking optional float with a value
void test_some_float() {
  Optional_float opt = some_float(3.14f);
  assert(is_some_float(opt) == true);
  assert(unwrap_float(opt) == 3.14f);
  printf("test_some_float passed\n");
}

// Test creating an optional float with no value
void test_none_float() {
  Optional_float opt = none_float();
  assert(is_some_float(opt) == false);
  printf("test_none_float passed\n");
}

// Test unwrapping a none value should crash (uncomment to test crash behavior)
// void test_unwrap_none_int_crash() {
//     Optional_int opt = none_int();
//     unwrap_int(opt);  // This should exit the program
// }

// Test creating and checking optional char with a value
void test_some_char() {
  Optional_char opt = some_char('a');
  assert(is_some_char(opt) == true);
  assert(unwrap_char(opt) == 'a');
  printf("test_some_char passed\n");
}

// Test creating an optional char with no value
void test_none_char() {
  Optional_char opt = none_char();
  assert(is_some_char(opt) == false);
  printf("test_none_char passed\n");
}

// Test modifying the optional value
void test_modify_optional_value() {
  Optional_int opt = some_int(20);
  assert(is_some_int(opt) == true);
  assert(unwrap_int(opt) == 20);

  opt.value = 30;
  assert(unwrap_int(opt) == 30);
  printf("test_modify_optional_value passed\n");
}

// Test changing from some to none
void test_change_some_to_none() {
  Optional_int opt = some_int(15);
  assert(is_some_int(opt) == true);

  // Change to none
  opt = none_int();
  assert(is_some_int(opt) == false);
  printf("test_change_some_to_none passed\n");
}

// Test optional behavior with different values
void test_multiple_optionals() {
  Optional_int opt1 = some_int(100);
  Optional_int opt2 = none_int();

  assert(is_some_int(opt1) == true);
  assert(is_some_int(opt2) == false);
  assert(unwrap_int(opt1) == 100);
  printf("test_multiple_optionals passed\n");
}

// Main test runner function
int main() {
  // Run individual tests
  test_some_int();
  test_none_int();
  test_some_float();
  test_none_float();
  // test_unwrap_none_int_crash();  // Uncomment to test crash behavior
  test_some_char();
  test_none_char();
  test_modify_optional_value();
  test_change_some_to_none();
  test_multiple_optionals();

  printf("All tests passed!\n");
  return 0;
}
