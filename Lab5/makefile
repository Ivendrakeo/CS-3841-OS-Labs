# gnu c compiler
#CC = gcc

# `Wall`    - warns about questionable things
# `Werror`  - makes all warnings errors
# `Wextra`  - enables some extra warning flags that `all` doesn't set
# `Wunused` - complains about any variable, function, label, etc. not being used
#CFLAGS = -Wall -Werror -Wextra -Wunused
# `g`           - generate source code debug info
# `std=`        - sets the language standard, in this case c99
# `_GNU_SOURCE` - is a macro that tells the compiler to use rsome gnu functions
# `pthred`      - adds support for multithreading with the pthreads lib (for preprocessor
#                 and linker)
# `O3`          - the level of optimization
#CFLAGS += -g -std=c99 -D_GNU_SOURCE -pthread -O3
# `-I` - adds directory to the system search path (for include files)
#CFLAGS += -I"$(INCDIR)"


CC=gcc
CFLAGS=-c -Wall -pthread
LDFLAGS=-pthread
SOURCES=llist.c Main.c BurgerPlace.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=test

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf $(OBJECTS) $(EXECUTABLE)
