c ?= gcc
cflags += -c -Wall -Werror -Iinclude
lflags +=

build ?= build

.PHONY: help
help:
	@echo "TODO: add help"

.PHONY: all
all: src test-comm

.PHONY: src
src:
	@src="src/entry.c" exe="made-to-race" $(MAKE) create

.PHONY: test-comm
test-comm:
	@src="test/comm/server.c test/comm/comm.c" exe="test/comm/server" cflags="-Itest/comm/" $(MAKE) create

.PHONY: watch
watch:
	watchexec -w . --exts c,h -- 'make -Bnwk all | compiledb'

obj = $(src:.c=.o)

.PHONY: create
create: $(obj)
	$(c) -o $(exe) $(obj) $(lflags)

%.o: %.c
	mkdir -p $(build)/$(dir $@)
	$(c) -o $@ $< $(cflags)
