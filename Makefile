# compiler settings
CC     = gcc
CFLAGS = -Wall -c -I.
LFLAGS = 

# sources to compile
SOURCES  = $(notdir $(wildcard ./*.c))

OBJDIR  = ./Objects
OBJECTS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

BIN = hexfile_merger

all: $(OBJDIR) $(SOURCES) $(BIN)

# link binary
$(BIN): $(OBJECTS)
	$(CC) $(LFLAGS) $(OBJECTS) -o $(BIN)

# compile source files
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $? -o $@

# create directory for objects
$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -fr $(OBJDIR)
	rm -fr $(BIN)
	

