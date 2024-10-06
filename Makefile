TARGET = simulation
BIN_DIR = bin
BUILD_DIR = build
SRC_DIR = src
INCLUDE_DIR = include

CC = gcc
CFLAGS = -Wall -Wextra -std=c11

# MacOS specific Raylib flags
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL