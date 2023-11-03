CC = g++
OBJ = rc.o Token.o

.PHONY: rc clean

rc: main.cpp
	${CC} -g -std=c++20 main.cpp Token.h -o rhtml 

clean:
