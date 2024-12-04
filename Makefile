OBJECTS = as.o lexer.o parser.o
CFLAGS  = -Wall -Wextra -g

TARGET = as

VALID_TESTS = $(wildcard tests/valid/*.s)
INVALID_TESTS = $(wildcard tests/invalid/*.s)

${TARGET}: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@

tests: $(TARGET) invalid_tests valid_tests

valid_tests:
	@for test in $(VALID_TESTS); do \
		if !(./$(TARGET) $$test > /dev/null); then \
			echo Failed test: $$test; \
		fi \
	done

invalid_tests:
	@for test in $(INVALID_TESTS); do \
		if ./$(TARGET) $$test > /dev/null 2>&1; then \
			echo Failed test: $$test; \
		fi \
	done

%.o: %.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

.PHONY: clean valid_tests invalid_tests tests
