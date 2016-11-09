# Makefile for using klee with docker
DOCKER=docker
START_DIR=$(shell echo $PWD)
TARGET_DIR=/home/klee/test
CONTAINER_NAME=klee_test_env

QUERY_ENV=$(DOCKER) ps -a | grep -o $(CONTAINER_NAME)
CREATE_ENV=$(DOCKER) run -v $(START_DIR):$(TARGET_DIR) -it -d --name=$(CONTAINER_NAME)  klee/klee
ENV_EXECUTE=$(DOCKER) exec $(CONTAINER_NAME)
CLEANUP_ENV=$(DOCKER) stop $(CONTAINER_NAME) && $(DOCKER) rm $(CONTAINER_NAME)

%.bc: %.c
	@ifneq ($(shell $(QUERY_ENV)),$(CONTAINER_NAME))
	@$(CREATE_ENV)
	@endif
	@echo " CLANG $^"
	@$(ENV_EXECUTE) cd test
	@$(ENV_EXECUTE) clang -emit-llvm -g -c $^ -o $@

%.klee: %.bc
	@echo "  KLEE $^"
	@$(ENV_EXECUTE) klee --libc=uclibc --posix-runtime $^
	#@$(ENV_EXECUTE) mv 

.PHONY: clean
clean:
	$(CLEANUP_ENV)
