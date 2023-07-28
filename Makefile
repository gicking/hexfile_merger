# compiler settings
CC     = gcc
CFLAGS = -Wall -I./include -g
#CFLAGS += -DMEMIMAGE_DEBUG					# activate optional debug output for memory image
#CFLAGS += -DMEMIMAGE_CHK_INCLUDE_ADDRESS	# include addresses into ckecksum over memory image
LFLAGS = -lm

# OS-dependent delete commands for 'make clean'
ifeq ($(OS),Windows_NT)
	RM = cmd //C del //Q //F
	RD = cmd //C rmdir //Q //S
else
	RM = rm -f
	RD = rm -fr
endif


# sources to compile
SRCDIR  = ./src
SOURCES  = $(notdir $(wildcard $(SRCDIR)/*.c))

OBJDIR  = ./lib
OBJECTS := $(addprefix $(OBJDIR)/, $(SOURCES:.c=.o))

BIN = hexfile_merger
BINARGS = -v 3 -import output/test.s19 -export output/test.s19

all: $(OBJDIR) $(BIN)

# create directory for objects
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

#	$(CC) $(LFLAGS) $^ -o $@
$(BIN): $(OBJECTS)
	$(CC) $(OBJECTS) $(LFLAGS) -o $@

clean:
	$(RM) $(OBJDIR)/*
	$(RM) -fr $(BIN)
	$(RD) -fr .pio/*

memcheck:
	valgrind --tool=memcheck --leak-check=full --show-leak-kinds=all -s ./$(BIN) $(BINARGS)
