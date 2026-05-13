CC = clang
VERSION = 2.0.0
CFLAGS = -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L -Isrc -MMD -MP -DGIG_VERSION=\"$(VERSION)\"
LDFLAGS = 

TARGET = build/gig
OBJ_DIR = build/obj

# Collect all source files
SRCS = src/main.c \
       src/parser/parser.c \
       src/parser/lexer.c \
       src/parser/metadata.c \
       src/parser/doc.c \
       src/layout/builder.c \
       src/layout/layout.c \
       src/layout/table.c \
       src/layout/utils.c \
       src/layout/wrap.c \
       src/layout/inline.c \
       src/pager/pager.c \
       src/pager/render.c \
       src/pager/input.c \
       src/pager/terminal.c \
       src/pager/buffer.c \
       src/cli/locator.c

OBJS = $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

.PHONY: all clean dirs

all: dirs $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

$(OBJ_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

-include $(DEPS)

dirs:
	@mkdir -p build
	@mkdir -p $(OBJ_DIR)

clean:
	rm -rf build
