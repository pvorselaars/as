SOURCES	  = $(wildcard *.c)
TOOLS     = $(basename $(SOURCES))

.PHONY: clean

all: $(TOOLS)

%: %.c
	$(CC) -Wall -g $^ -o $@

clean:
	rm $(TOOLS)
