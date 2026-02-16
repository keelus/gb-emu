CC=g++
CFLAGS=-std=c++11 -g -O0

SOURCES=\
		  src/main.cpp \
		  src/cpu.cpp \
		  src/gameboy.cpp \
		  src/memory.cpp
NAME=gb-emu

build:
	$(CC) $(CFLAGS) $(SOURCES) -o $(NAME)

run:
	./$(NAME)

clean:
	rm -f $(NAME)
