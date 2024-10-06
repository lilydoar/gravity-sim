TARGET = simulation
BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

# External libraries
RAYLIB_DIR = extern/raylib
RAYGUI_DIR = extern/raygui

# MacOS specific Raylib flags
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 $(shell find $(INCLUDE_DIR) -type d | sed 's/^/-I/') -I$(RAYLIB_DIR)/include -I$(RAYGUI_DIR)/include
LDFLAGS = -L$(RAYLIB_DIR)/lib -lraylib -lm

# Find all .c files recursively
SRCS = $(shell find $(SRC_DIR) -name '*.c')
# Generate object file names, preserving directory structure
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LFLAGS)

# Rule for building object files, creating directories as needed
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
