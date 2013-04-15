EXEC=ace
SRCS=$(wildcard src/*.cpp src/*/*.cpp src/*/*.cpp src/*/*/*.cpp src/*/*/*/*.cpp)
OBJS=$(SRCS:.cpp=.o)
DEPS=$(OBJS:.o=.d)
INCS=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`

CC=g++
CPPFLAGS = -std=c++11 -Wall -pthread -g

#%.o: %.cpp
#	$(CC) $(CPPFLAGS) $(INCS) -c $< -o $@

all: $(EXEC)

-include $(DEPS)

%.o: %.cpp
	$(CC) $(CPPFLAGS) $(INCS) -MMD -c $< -o $@
 


# ================================
$(EXEC): $(OBJS)
	$(LINK.o) $^ -o $@ $(LIBS)
clean:
	rm -f $(OBJS) ./.depend $(DEPS)
run:
	./$(EXEC)


#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
