# compiler settings
CC     = gcc
CFLAGS = -Wall -I. -g
LFLAGS = -lm

# sources to compile
SOURCES  = $(notdir $(wildcard ./*.c))

OBJDIR  = ./Objects
OBJECTS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

BIN = hexfile_merger
BINARGS = -v 3 -import output/test.s19 -export output/test.s19

all: $(OBJDIR) $(SOURCES) $(BIN)

# link binary
$(BIN): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

# compile source files
$(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $? -o $@

# create directory for objects
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -fr $(OBJDIR)
	rm -fr $(BIN)

memcheck:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all -s ./$(BIN) $(BINARGS)
