CCFLAGS = -Werror -std=c99

LINK_FLAGS = -lm

SOURCES=$(wildcard src/*.c) $(wildcard hashmap.c/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: CCFLAGS += -O3
all: lineset

debug: CCFLAGS += -g3
debug: lineset


lineset: $(OBJECTS)
	$(CC) $(LINK_FLAGS) $(CCFLAGS) $(OBJECTS) -o lineset

src/%.o: src/%.c
	$(CC) $(CCFLAGS) -c $^ -o $@

hashmap.c/%.o: hashmap.c/%.c
	$(CC) $(CCFLAGS) -c $^ -o $@

clean:
	rm -rf lineset*
	rm -f src/*.o
	rm -f hashmap.c/hashmap.o

install: lineset
	cp lineset /usr/local/bin/

.BOGUS:
