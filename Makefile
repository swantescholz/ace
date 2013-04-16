#%.o: %.cpp
#	$(CC) $(CPPFLAGS) $(INCS) -c $< -o $@
#
EXEC=ace
INCS=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`
FLAGS=-std=c++11 -Wall -pthread
CC=g++
CPPFLAGS=$(FLAGS) $(INCS)


BUILDDIR=build
SUBDIRS=$(wildcard src src/* src/*/* src/*/*/*/* src/*/*/*/*/* src/*/*/*/*/*/*)
SRCS=$(wildcard $(addsuffix /*.cpp,$(SUBDIRS)))
OBJS=$(SRCS:.cpp=.o)

# ================================

all: $(EXEC)

# ================================

depend: .depend

.depend:
	rm -f ./.depend
	for src in $(SRCS) ; do \
		echo $$src; \
		thename=$$src | sed "s/\([[:print:]]*\).cpp/\1/"
		$(CC) $(FLAGS) -MM -MT $(thename).o $(thename).cpp \
			| sed "s/\([[:print:]]*\).cpp: \([[:print:]]*\)/\1.o: \1.cpp \2/" \
			>> ./.depend; \
	done

-include .depend

# ================================
clean:
	@echo helllo
	rm -f -R $(BUILDDIR)/* $(OBJS) .depend
run:
	./$(EXEC)
$(EXEC): $(OBJS)
	@echo exec done!
	$(LINK.o) $^ -o $@ $(LIBS)



#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
