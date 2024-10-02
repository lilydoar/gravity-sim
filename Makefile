CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/ -Itests/unity/Unity-2.6.0/src
LIB = -Llib/ -lraylib
TARGET = bin/gravity-sim
SOURCES = src/main.c src/gravity.c src/gravity_interactor.c src/arena_allocator.c src/ui_handler.c src/verlet.c src/logging.c

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
TEST_TARGET = bin/test_gravity_interactor

$(TEST_TARGET): $(TEST_SOURCES) $(UNITY_SRC)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TEST_TARGET) $(TEST_SOURCES) $(UNITY_SRC)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: test
