EXEC=ace
SRC=$(wildcard src/*.cpp src/*/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
OBJ=$(SRC:.cpp=.o)
INC=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`

CC=g++
CCOPTS=-std=c++11 -c -Wall -pthread

all: $(EXEC)

$(EXEC): $(OBJ)
	$(LINK.o) $^ -o $@ $(LIBS)

.cpp.o :
	$(CC) $(CCOPTS) $(INC) -c $< -o $@

clean:
	rm $(EXEC) $(OBJ)
run:
	./$EXEC


#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
