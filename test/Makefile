## Variables ##
# Location of all the objects to be tested
BUILD_PREFIX=../
# Test cases to build and execute
TESTS=

.PHONY: all run clean

## Targets ##
all: $(TESTS)

# Run all tests
run: $(addprefix run_, $(TESTS))

# Run single test
run_%: %
	./$*

# Clean up
clean:
	-rm -f $(TESTS)
	-rm -f $(addsuffix .o, $(TESTS))
	-rm -f $(addsuffix .d, $(TESTS))

# Build remote objects
$(BUILD_PREFIX)%:
	$(MAKE) -D $(BUILD_PREFIX) $*

## Automatic dependencies ##
%.d: %.c
	$(CC) -MM $^ -D_FILE_OFFSET_BITS=64 > $@

include $(addsuffix .d, $(TESTS))

## Manual dependencies ##
