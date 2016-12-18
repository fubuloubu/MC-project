include bcflags.mk

%.i: %.c
	@echo "  CLANG -E $^"
	@clang $(BCFLAGS) -E $^ > $@

%.bc: %.i
	@echo "  CLANG -c $^"
	@clang -emit-llvm -c $^ -o $@

# Doing it this way instead of symlink to Makefile above
.PHONY: status
status:
	@$(MAKE) -f ../Makefile status

.PHONY: clean
clean:
	@echo "  CLEAN usbmouse"
	@rm -f *-preprocessed.c 
	@rm -f *.i
	@rm -f *.s
	@rm -f *.bc 
