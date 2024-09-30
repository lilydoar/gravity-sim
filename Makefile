CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/
LIB = -Llib/ -lraylib
TARGET = bin/gravity-sim
SOURCES = src/main.c src/gravity.c src/gravity_interactor.c src/arena_allocator.c src/ui_handler.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(LIB) -o $(TARGET) $(SOURCES)

run: $(TARGET)
	./$(TARGET)

clean:
	rm $(TARGET)

TEST_SOURCES = tests/test_gravity_interactor.c src/gravity_interactor.c src/arena_allocator.c
TEST_TARGET = bin/test_gravity_interactor

$(TEST_TARGET): $(TEST_SOURCES)
	$(CC) $(CFLAGS) $(INCLUDE) -o $(TEST_TARGET) $(TEST_SOURCES)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

.PHONY: test
