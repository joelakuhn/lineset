CCFLAGS = -Werror -std=c99

LINK_FLAGS = -lm

SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)

all: CCFLAGS += -O3
all: lineset

debug: CCFLAGS += -g3
debug: lineset

profile: CCFLAGS += -g3 -pg
profile: lineset

lineset: $(OBJECTS)
	$(CC) $(LINK_FLAGS) $(CCFLAGS) $(OBJECTS) -o lineset

src/%.o: src/%.c
	$(CC) $(CCFLAGS) -c $^ -o $@

clean:
	rm -rf lineset*
	rm -f src/*.o

install: lineset
	cp lineset /usr/local/bin/

.BOGUS:
