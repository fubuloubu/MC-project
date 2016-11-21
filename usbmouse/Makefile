include bcflags.mk

# Compiler stage (originally -c) TODO: Try -S
BCFLAGS+=-c

# Generate Bytecode (and store in file -o)
BCFLAGS+=-emit-llvm

%.bc: %.c
	@echo "  CLANG $<"
	@clang $(BCFLAGS) -o $@ $^

.PHONY: clean
clean:
	@echo "  CLEAN clang"
	@rm -f usbmouse.bc 
