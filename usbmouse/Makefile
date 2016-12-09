include bcflags.mk

# Compiler stage (originally -c)
BCFLAGS+=-c

# Generate Bytecode (and store in file -o)
BCFLAGS+=-emit-llvm

usbmouse.bc: usbmouse.c
	@echo "  CLANG usbmouse"
	@clang $(BCFLAGS) -o $@ $^

usbmouse-modified.bc: usbmouse-modified.c
	@echo "  CLANG usbmouse"
	@clang -emit-llvm -c -o $@ $^

# Doing it this way instead of symlink to Makefile above
.PHONY: status
status:
	@$(MAKE) -f ../Makefile status

.PHONY: clean
clean:
	@echo "  CLEAN usbmouse"
	@rm -f usbmouse*.bc 
