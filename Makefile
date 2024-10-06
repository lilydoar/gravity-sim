# Define required raylib variables
PROJECT_NAME          ?= simulation

# Create build and bin directories
BUILD_DIR = build
BIN_DIR = bin

# Define paths to external libraries
CGLM_PATH             ?= extern/cglm
RAYLIB_PATH           ?= extern/raylib
RAYGUI_PATH           ?= extern/raygui

# Define compiler flags: CFLAGS
CFLAGS = -Wall -std=c99 -D_DEFAULT_SOURCE -Wno-missing-braces -DCGLM_USE_DOUBLE -DRAYGUI_IMPLEMENTATION 

# Define include paths for required headers: INCLUDE_PATHS
INCLUDE_PATHS = -I. -I$(CGLM_PATH)/include -I$(RAYLIB_PATH)/include  -Iinclude -I$(RAYGUI_PATH)/src 

# Define library paths containing required libs: LDFLAGS
LDFLAGS = -L$(CGLM_PATH)/lib -L$(RAYLIB_PATH)/lib 

# Define libraries required on linking: LDLIBS
# Adjusted for macOS
LDLIBS = -lcglm -lraylib -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo

# Define source files
SOURCES = $(wildcard src/*.c)
OBJECTS = $(patsubst src/%.c,build/%.o,$(SOURCES))

# Define the target executable
TARGET = bin/$(PROJECT_NAME)

# Default target
all: create_dirs $(TARGET)

# Create necessary directories
create_dirs:
	mkdir -p $(BUILD_DIR) $(BIN_DIR)

# Linking the executable
$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS) $(LDLIBS)

# Compiling source files
$(BUILD_DIR)/%.o: src/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS)

# Run the executable
run: all
	./$(TARGET)

# Cleaning up
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean run create_dirs