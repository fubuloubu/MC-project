IGNORE=/dev/null

# Makefile for using klee with docker
DOCKER=docker
START_DIR=$(shell pwd)
TARGET_DIR=/home/klee/test
CONTAINER_NAME=klee_test

# Rule for creating environment (NOTE: keep persistant)
.SECONDARY: $(CONTAINER_NAME)
$(CONTAINER_NAME):
	@echo " CREATE $(CONTAINER_NAME)"
	@$(DOCKER) run -v $(START_DIR):$(TARGET_DIR) \
		-it -d --name=$(CONTAINER_NAME) \
		klee/klee 2>$(IGNORE) >$(IGNORE)
	@touch $@

# Command to execute inside environment
ENV_EXECUTE=$(DOCKER) exec $(CONTAINER_NAME)

# Setup clang to generate bytecode for use with klee
CLANG_FLAGS =-c
CLANG_FLAGS+=-g
CLANG_FLAGS+=-emit-llvm
CLANG_FLAGS+=-I /home/klee/klee_src/include/
CLANG=clang $(CLANG_FLAGS)

# Generate LLVM bytecode for use with klee
%.bc: %.c $(CONTAINER_NAME)
	@echo "  CLANG $<"
	@$(ENV_EXECUTE) $(CLANG) $(TARGET_DIR)/$< -o $(TARGET_DIR)/$@

# Setup klee to generate test cases
KLEE=klee --libc=uclibc --posix-runtime

# Run klee to get test cases and execute them
%.results: %.bc $(CONTAINER_NAME)
	@echo "   KLEE $<"
	@$(ENV_EXECUTE) $(KLEE) $(TARGET_DIR)/$< 2>$(IGNORE)
	@cd klee-out-0 && for file in test*.ktest; do \
		$(ENV_EXECUTE) ktest-tool $(TARGET_DIR)/klee-last/$$file >> $@; \
	 done
	@mv klee-out-0/$@ $@

# Remove environment
destroy_env:
	@echo "DESTROY $(CONTAINER_NAME)"
	@$(DOCKER) stop $(CONTAINER_NAME) 2>$(IGNORE) >$(IGNORE)
	@$(DOCKER) rm   $(CONTAINER_NAME) 2>$(IGNORE) >$(IGNORE)
	@rm $(CONTAINER_NAME)

# Clean rule:
# clean using executeable lines of .gitignore file
.PHONY: clean
clean:
	@if [ -f $(CONTAINER_NAME) ]; then $(MAKE) -s destroy_env; fi
	@echo "  CLEAN klee/.gitignore"
	@grep -v "#\|^\$$" .gitignore | while read line; do rm -rf $$line; done
