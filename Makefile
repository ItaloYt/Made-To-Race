pkgconfig ?= pkg-config

c ?= gcc
cflags += -Wall -Werror -Iinclude -MMD
lflags +=

sc ?= glslc
scflags += -MD
slflags +=

build ?= build

obj = $(addprefix $(build)/,$(src:.c=.o))
dep = $(obj:.o=.d)

src := \
	core/entry.c \
	core/graphic.c \
	core/window.c \
	core/shaders.c

ssrc := \
	shader/base.vert \
	shader/base.frag

sobj = $(addprefix $(build)/,$(addsuffix .spv,$(ssrc)))
sdep = $(sobj:.spv=.d)

.PHONY: help
help:
	@echo "TODO: add help"

# GRAPHIC BACKEND
graphic := $(shell $(pkgconfig) --exists vulkan && echo "true")

.PHONY: graphic
ifeq ($(graphic), true)
cflags += -DMTR_VULKAN=1
lflags += -lvulkan

src += \
	backend/graphic/vulkan/graphic.c \
	backend/graphic/vulkan/instance.c \
	backend/graphic/vulkan/surface.c \
	backend/graphic/vulkan/physical.c \
	backend/graphic/vulkan/device.c \
	backend/graphic/vulkan/swapchain.c \
	backend/graphic/vulkan/image.c \
	backend/graphic/vulkan/shader.c

graphic:
else
$(error Unsupported graphic backend)
endif

# WINDOW BACKEND
window := $(shell $(pkgconfig) --exists wayland-client && echo "true")

.PHONY: window
ifeq ($(window), true)
cflags += -DMTR_WAYLAND=1
lflags += -lwayland-client

xdg-shell ?= /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
xdg-decoration ?= /usr/share/wayland-protocols/unstable/xdg-decoration/xdg-decoration-unstable-v1.xml

src += \
	backend/window/wayland/window.c \
	backend/window/wayland/display.c \
	backend/window/wayland/registry.c \
	backend/window/wayland/surface.c \
	backend/window/wayland/xdg-shell.c \
	backend/window/wayland/xdg-decoration.c

window: include/backend/window/wayland/xdg-shell.h backend/window/wayland/xdg-shell.c include/backend/window/wayland/xdg-decoration.h backend/window/wayland/xdg-decoration.c

include/backend/window/wayland/xdg-shell.h:
	@wayland-scanner client-header $(xdg-shell) $@

backend/window/wayland/xdg-shell.c:
	@wayland-scanner private-code $(xdg-shell) $@

include/backend/window/wayland/xdg-decoration.h:
	@wayland-scanner client-header $(xdg-decoration) $@

backend/window/wayland/xdg-decoration.c:
	@wayland-scanner private-code $(xdg-decoration) $@

else
$(error Unsupported window backend)
endif

$(build)/%.o: %.c
	@mkdir -p $(dir $@)
	$(c) -c -o $@ $< $(cflags)

$(build)/made-to-race: $(obj)
	@mkdir -p $(dir $@)
	$(c) -o $@ $^ $(lflags)

include/game/shaders.h: $(ssrc)
	@echo -e "#ifndef MTR_SHADERS_H\n#define MTR_SHADERS_H\n" > $@
	@for file in $(ssrc); do \
		name="$$(echo $$file | tr '/' '_' | tr '.' '_')_spv"; \
		echo -e "extern const char $$name[];\nextern const unsigned $${name}_size;\n" >> $@; \
	done
	@echo -e "#endif\n" >> $@

core/shaders.c: $(ssrc)
	@echo -e '#include "game/shaders.h"\n' > $@
	@for file in $(ssrc); do \
		name="$$(echo $$file | tr '/' '_' | tr '.' '_')_spv"; \
		content="$$(xxd -ps $(build)/$$file.spv | tr -d '\n' | sed 's/../\\\\x&/g')"; \
		echo -e "const char $$name[] = \"$$content\";\nconst unsigned $${name}_size = sizeof($$name) - 1;\n" >> $@; \
	done

$(build)/%.spv: %
	@mkdir -p $(dir $@)
	$(sc) -o $@ $< $(scflags) $(slflags)

shaders: $(sobj) core/shaders.c include/game/shaders.h

.PHONY: build
build: window graphic shaders $(build)/made-to-race

.PHONY: test
test:
	@build="$(abspath $(build))/test" root="$(abspath ./)" $(MAKE) -C test/comm/ build

.PHONY: all
all: build test

.PHONY: watch
watch:
	watchexec -e c,h,vert,frag -w . -- "make -Bnwk all | compiledb"

-include $(dep)
-include $(sdep)
