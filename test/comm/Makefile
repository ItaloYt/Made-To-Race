c ?= gcc
cflags += -Wall -Werror
lflags +=

build ?= build

obj = $(addprefix $(build)/,$(src:.c=.o))
dep = $(obj:.o=.d)

src := \
	comm.c \
	server.c

.PHONY: build
build: $(build)/comm

$(build)/comm: $(obj)
	@mkdir -p $(dir $@)
	$(c) -o $@ $^ $(lflags)

$(build)/%.o: %.c
	@mkdir -p $(dir $@)
	$(c) -c -o $@ $< $(cflags)

-include $(dep)
