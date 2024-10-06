TARGET = simulation
BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I$(INCLUDE_DIR)
LDFLAGS = -lraylib -lm

# MacOS specific Raylib flags
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(BIN_DIR)/$(TARGET)

$(BIN_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR) $(BUILD_DIR)
