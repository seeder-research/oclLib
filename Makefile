CC      = gcc
AR      = ar

# Target OpenCL 1.2 and suppress deprecation warnings for 1.x APIs
# when building against newer Khronos headers.
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic -O2 \
          -DCL_TARGET_OPENCL_VERSION=120 \
          -I include \
          -I external/OpenCL-Headers

LDFLAGS = -lOpenCL

SRC_DIR   = src
BUILD_DIR = build
LIB       = $(BUILD_DIR)/liboclLib.a

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean

all: $(LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB): $(OBJS)
	$(AR) rcs $@ $^

clean:
	rm -rf $(BUILD_DIR)
