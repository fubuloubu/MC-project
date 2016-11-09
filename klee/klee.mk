IGNORE=/dev/null

# Makefile for using klee with docker
DOCKER=docker
START_DIR=$(shell pwd)
TARGET_DIR=/home/klee/test
CONTAINER_NAME=klee_test_env

QUERY_ENV=$(DOCKER) ps -a | grep -o $(CONTAINER_NAME)
CREATE_ENV=$(DOCKER) run -v $(START_DIR):$(TARGET_DIR) -it -d --name=$(CONTAINER_NAME) \
	klee/klee 2>$(IGNORE) >$(IGNORE)
ENV_EXECUTE=$(DOCKER) exec $(CONTAINER_NAME)
CLEANUP_ENV=$(DOCKER) stop $(CONTAINER_NAME) 2>$(IGNORE) >$(IGNORE)\
	    && $(DOCKER) rm $(CONTAINER_NAME) 2>$(IGNORE) >$(IGNORE)

%.bc: %.c
ifneq ($(shell $(QUERY_ENV)),$(CONTAINER_NAME))
	@echo " CREATE $(CONTAINER_NAME)"
	@$(CREATE_ENV)
endif
	@echo " CLANG $^"
	@$(ENV_EXECUTE) clang -emit-llvm -g -c $(TARGET_DIR)/$^ -o $@
	@$(ENV_EXECUTE) mv $@ $(TARGET_DIR)

%.klee: %.bc
	@echo "  KLEE $^"
	@$(ENV_EXECUTE) klee --libc=uclibc --posix-runtime $(TARGET_DIR)/$^ 2>$(IGNORE)

# NOTE: Cannot execute clean and above rules at the same time due to conditionals
.PHONY: clean
clean:
ifeq ($(shell $(QUERY_ENV)),$(CONTAINER_NAME))
	@echo "DESTROY $(CONTAINER_NAME)"
	@$(CLEANUP_ENV)
endif
	@echo " CLEAN klee"
	@rm -f *.bc
	@rm -f klee-last
	@rm -rf klee-out-*
