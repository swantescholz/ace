EXEC=ace
INCS=`pkg-config --cflags gtkmm-3.0` -I/usr/include/gtksourceview-3.0
LIBS=-L/usr/lib -L/usr/local/lib `pkg-config --libs gtkmm-3.0` `pkg-config --libs gtksourceview-3.0`
BUILDDIR=build
SUBDIRS := $(wildcard src/* src/*/* src/*/*/*/* src/*/*/*/*/* src/*/*/*/*/*/*)
SRCS=$(wildcard $(addsuffix /*.cpp,$(SUBDIRS)))
#OBJS := $(addprefix $(BUILDDIR)/,$(SRCS:.cpp=.o))
OBJS=$(SRCS:.cpp=.o)
DEPS=$(BUILDDIR)/`echo $(OBJS) | sed "s/\.o/\.d/"`

CC=g++
CPPFLAGS = -std=c++11 -Wall -pthread -g

#%.o: %.cpp
#	$(CC) $(CPPFLAGS) $(INCS) -c $< -o $@

tmp:
	@echo $(OBJS) $(DEPS)

all: $(EXEC)
	

-include $(DEPS)

%.o: %.cpp
	@echo $<
	@echo $@
	$(CC) $(CPPFLAGS) $(INCS) -MMD \
		-MF $(BUILDDIR)/`echo $@ | sed "s/\.o/\.d/"` \
		-c $< -o $(BUILDDIR)/$@
 


# ================================
$(EXEC): $(OBJS)
	$(LINK.o) $^ -o $@ $(LIBS)
clean:
	rm -f -R $(BUILDDIR)/*
run:
	./$(EXEC)


#Special symbols used:
#$^ - is all the dependencies (in this case =$(OBJECTS) )
#$@ - is the result name (in this case =$(EXECUTABLE) )
