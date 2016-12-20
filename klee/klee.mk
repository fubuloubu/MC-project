IGNORE=/dev/null

# Makefile for using klee with docker
DOCKER=docker
CONTAINER_NAME=klee/klee:linux
CONTAINER_EXISTS=$(shell $(DOCKER) images | grep "$(subst :, *,$(CONTAINER_NAME))")

.PHONY: build
build:
	@echo " BUILD $(CONTAINER_NAME)"
	@$(if $(CONTAINER_EXISTS),$(DOCKER) rmi $(CONTAINER_NAME) >$(IGNORE))
	@$(DOCKER) build -t "$(CONTAINER_NAME)" .

START_DIR=$(shell pwd)
TARGET_DIR=/home/klee/test
SOURCE_DIR=/home/klee/usbmouse
INSTANCE_NAME=klee_test

# Rule for creating environment (NOTE: keep persistant)
.SECONDARY: $(INSTANCE_NAME)
$(INSTANCE_NAME): $(if $(CONTAINER_EXISTS),,build)
	@echo " CREATE $(INSTANCE_NAME)"
	@$(DOCKER) run \
		-v $(shell pwd):$(TARGET_DIR) \
		-v $(shell pwd)/../usbmouse:$(SOURCE_DIR) \
		-it -d --name=$(INSTANCE_NAME) $(CONTAINER_NAME) \
		2>$(IGNORE) >$(IGNORE)
	@touch $@

# Command to execute inside environment
ENV_EXECUTE=$(DOCKER) exec $(INSTANCE_NAME)

# Setup clang to generate bytecode for use with klee
CLANG_FLAGS+=-I /home/klee/klee_src/include/
CLANG_FLAGS+=-emit-llvm
CLANG_FLAGS+=-c
CLANG=clang $(CLANG_FLAGS)

# Get preprocessed source for harness from GCC
usbmouse-harness.c: ../usbmouse/usbmouse.i $(INSTANCE_NAME)
../usbmouse/usbmouse.i: $(INSTANCE_NAME)
	@echo "   MAKE usbmouse"
	@$(ENV_EXECUTE) bash -c "cd $(SOURCE_DIR) && $(MAKE) all 2>$(IGNORE) >$(IGNORE)"

# Generate LLVM bytecode for use with klee
%.bc: %.c $(INSTANCE_NAME)
	@echo "  CLANG $<"
	@$(ENV_EXECUTE) $(CLANG) $(TARGET_DIR)/$< -o $(TARGET_DIR)/$@ 2>$(IGNORE)

# Setup klee to generate test cases
KLEE_FLAGS=--libc=uclibc
KLEE=klee $(KLEE_FLAGS)

# Run klee to get test cases and execute them
%.results: %.bc $(INSTANCE_NAME)
	@echo "   KLEE $<"
	@$(ENV_EXECUTE) $(KLEE) $(TARGET_DIR)/$<
	@cd klee-out-0 && for file in test*.ktest; do \
		$(ENV_EXECUTE) ktest-tool $(TARGET_DIR)/klee-last/$$file >> $@; \
	 done
	@mv klee-out-0/$@ $@

# Remove environment
destroy_env:
	@echo "DESTROY $(INSTANCE_NAME)"
	@$(DOCKER) stop $(INSTANCE_NAME) 2>$(IGNORE) >$(IGNORE)
	@$(DOCKER) rm   $(INSTANCE_NAME) 2>$(IGNORE) >$(IGNORE)
	@rm $(INSTANCE_NAME)

# Clean rule:
# clean using executeable lines of .gitignore file
.PHONY: clean
clean:
	@if [ -f $(INSTANCE_NAME) ]; then \
		echo "   MAKE clean_usbmouse"; \
		$(ENV_EXECUTE) bash -c "cd $(SOURCE_DIR) && $(MAKE) clean >$(IGNORE)"; \
		$(MAKE) -s destroy_env; \
	 fi
	@echo "  CLEAN klee/.gitignore"
	@grep -v "#\|^\$$" .gitignore | while read line; do rm -rf $$line; done
