CC     ?= cc
CFLAGS ?= -Wall -Wextra

BINS = dispatch_sim syscall_probe

all: $(BINS)

%: %.c
	$(CC) $(CFLAGS) -o $@ $<

test: all
	@bash ./test.sh

clean:
	rm -f $(BINS)

.PHONY: all test clean
