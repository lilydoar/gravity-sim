CC = gcc
CFLAGS = -Wall -Wextra -std=c17
LFLAGS = -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL
INCLUDE = -Iinclude/
LIB = -Llib/ -lraylib
TARGET = bin/gravity-sim

all: $(TARGET)

$(TARGET): src/main.c src/gravity.c
	$(CC) $(CFLAGS) $(LFLAGS) $(INCLUDE) $(LIB) -o $(TARGET) src/main.c src/gravity.c

clean:
	rm $(TARGET)
