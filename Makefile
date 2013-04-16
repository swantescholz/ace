#%.o: %.cpp
#	$(CC) $(CPPFLAGS) $(INCS) -c $< -o $@
#
EXEC=ace
INCS=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`
CC=g++
CPPFLAGS=-std=c++11 -Wall -pthread $(INCS)


BUILDDIR=build
SUBDIRS=$(wildcard src src/* src/*/* src/*/*/*/* src/*/*/*/*/* src/*/*/*/*/*/*)
SRCS=$(wildcard $(addsuffix /*.cpp,$(SUBDIRS)))
OBJS=$(SRCS:.cpp=.o)

# ================================




all: $(EXEC)

#.o:
#	@echo mymdd
#	$(CC) $(CPPFLAGS) $(INCS) -c $< -o $@

# ================================

#depend: .depend

#.depend: $(SRCS)
#	rm -f ./.depend
#	$(CC) $(CPPFLAGS) $(INCS) -MM $^ > ./.depend;



# ================================
clean:
	@echo helllo
	rm -f -R $(BUILDDIR)/* $(OBJS)
run:
	./$(EXEC)
$(EXEC): $(OBJS)
	@echo exec done!
	$(LINK.o) $^ -o $@ $(LIBS)



#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
