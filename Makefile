EXEC=ace
SRC=$(wildcard src/*.cpp src/*/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
OBJ=$(SRC:.cpp=.o)
INC=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`

CC=g++
CCOPTS=-std=c++11 -Wall -pthread -g

#DEPDIR=build
#df = $(DEPDIR)/$(*F)
#MAKEDEPEND = g++ -M $(CCOPTS) $(INC) $(CPPFLAGS) -o $*.d $<
#.cpp.o :
#	$(CC) $(CCOPTS) $(INC) -c $< -o $@
# $(COMPILE.c) -MD $(CCOPTS) $(INC) -o $@ $<

%.d: %.c
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< | sed '\"s/$*.o/& $@/g'\" > $@'
-include $(SRC:.cpp=.d)

.cpp.o :
	$(CC) $(CCOPTS) $(INC) -c $< -o $@


all: $(EXEC)

$(EXEC): $(OBJ)
	$(LINK.o) $^ -o $@ $(LIBS)
	
clean:
	rm $(EXEC) $(OBJ)
run:
	./$(EXEC)



#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
