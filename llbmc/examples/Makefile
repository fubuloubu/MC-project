BC_FILES = double-free.bc memory-access.bc openssl.bc popcount.bc

compile: $(BC_FILES)
	@echo "\nYou can now run LLBMC on any of the example files, e.g."
	@echo "  llbmc double-free.bc"
	@echo "  llbmc memory-access.bc --max-loop-iterations=4"
	@echo "  llbmc popcount.bc"
	@echo "  llbmc openssl.bc --max-loop-iterations=5"

%.bc: %.c
	clang -c -g -emit-llvm -I. $< -o $@

clean:
	rm -rf $(BC_FILES)
