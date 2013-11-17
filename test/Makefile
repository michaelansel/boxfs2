## Variables ##
# Location of all the objects to be tested
BUILD_PREFIX=../
# Test cases to build and execute
TESTS=test_boxcache

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
	-$(RM) $(TESTS)
	-$(RM) $(addsuffix .o, $(TESTS))
	-$(RM) $(addsuffix .d, $(TESTS))

# Build remote objects (always)
$(BUILD_PREFIX)%.o: FORCE
	$(MAKE) -C $(BUILD_PREFIX) $*.o

FORCE:

## Automatic dependencies ##
%.d: %.c
	$(CC) -MM $^ -D_FILE_OFFSET_BITS=64 > $@

include $(addsuffix .d, $(TESTS))

## Manual dependencies ##
test_boxcache: $(addprefix $(BUILD_PREFIX), boxcache.o boxutils.o)
