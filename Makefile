CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/
LIB = -Llib/ -lraylib
TARGET = gravity-sim

all: $(TARGET)

$(TARGET): src/**.c
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(LIB) -o $(TARGET) src/main.c

clean:
	rm $(TARGET)
