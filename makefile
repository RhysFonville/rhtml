CC = g++
OBJ = rc.o Token.o

.PHONY: rc clean

rc: main.cpp
	${CC} -g -std=c++17 main.cpp Token.h -o rhtml 

clean:
