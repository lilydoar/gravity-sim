CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/ -Itests/unity/Unity-2.6.0/src
LIB = -Llib/ -lraylib
TARGET = bin/gravity-sim
SOURCES = $(shell find src -name '*.c') src/iterator.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(LIB) -o $(TARGET) $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

UNITY_DIR = tests/unity/Unity-2.6.0/src
UNITY_SRC = $(UNITY_DIR)/unity.c
TEST_SOURCES = tests/test_gravity_interactor.c src/gravity_interactor.c src/arena_allocator.c
TEST_GRAVITY_SOURCES = tests/test_gravity.c src/gravity.c src/verlet.c src/logging.c
TEST_UI_SOURCES = tests/test_ui_handler.c src/ui_handler.c src/arena_allocator.c src/gravity_interactor.c src/gravity.c src/logging.c src/verlet.c
TEST_TARGET = bin/test_gravity_interactor
TEST_GRAVITY_TARGET = bin/test_gravity
TEST_UI_TARGET = bin/test_ui_handler

$(TEST_TARGET): $(TEST_SOURCES) $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) $(LFLAGS) -o $(TEST_TARGET) $(TEST_SOURCES) $(UNITY_SRC)

$(TEST_GRAVITY_TARGET): $(TEST_GRAVITY_SOURCES) $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) $(LFLAGS) -o $(TEST_GRAVITY_TARGET) $(TEST_GRAVITY_SOURCES) $(UNITY_SRC)

$(TEST_UI_TARGET): $(TEST_UI_SOURCES) $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDE) $(LIB) $(LFLAGS) -o $(TEST_UI_TARGET) $(TEST_UI_SOURCES) $(UNITY_SRC)

check: $(TEST_TARGET) $(TEST_UI_TARGET) $(TEST_GRAVITY_TARGET)
	./$(TEST_TARGET)
	./$(TEST_UI_TARGET)

.PHONY: check
