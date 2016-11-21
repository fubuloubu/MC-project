include bcflags.mk

# Compiler stage (originally -c) TODO: Try -S
BCFLAGS+=-c

# Generate Bytecode (and store in file -o)
BCFLAGS+=-emit-llvm

usbmouse.bc: usbmouse.c
	@echo "  CLANG usbmouse"
	@clang $(BCFLAGS) -o $@ $^

.PHONY: clean
clean:
	@echo "  CLEAN usbmouse"
	@rm -f usbmouse.bc 
