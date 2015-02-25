CC = g++
LIBS = -lglut -lGL -lGLU

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

.PHONY: default all clean


all: BoulderBlast

%.o: %.cpp $(HEADERS)
	$(CC) -c -Wall -std=c++11 $< -o $@

BoulderBlast: $(OBJECTS) 
	$(CC) $(OBJECTS) $(LIBS) -o $@

clean:
	rm -f *.o
	rm -f BoulderBlast
