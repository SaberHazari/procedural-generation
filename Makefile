# Compiler
CC = clang

# Libraries
LIBS = -lm

# Directories
# Directory for Android CxStudio
PROJECT_DIR := $(HOME)/procedural-generation
SRC_DIR := src
BUILD_DIR := $(PROJECT_DIR)/build
DEBUG_DIR := $(BUILD_DIR)/debug
DEBUG_OBJ_DIR := $(DEBUG_DIR)/obj
RELEASE_DIR := $(BUILD_DIR)/release
RELEASE_OBJ_DIR := $(RELEASE_DIR)/obj

# Compiler Flags
COMMON_FLAGS = -Wall -Wextra -std=c17 -Isrc/
DEBUG_FLAGS = $(COMMON_FLAGS) -g -O0
RELEASE_FLAGS = $(COMMON_FLAGS) -O2

# Source Files
SRCS := $(shell find $(SRC_DIR) -type f -name '*.c')

# Object Files
DEBUG_OBJS := $(addprefix $(DEBUG_OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))
RELEASE_OBJS := $(addprefix $(RELEASE_OBJ_DIR)/, $(notdir $(SRCS:.c=.o)))

# Dependencies
DEBUG_DEPS := $(DEBUG_OBJS:%.o=%.d)
RELEASE_DEPS := $(RELEASE_OBJS:%.o=%.d)

# Binaries
DEBUG_BIN := $(DEBUG_DIR)/main
RELEASE_BIN := $(RELEASE_DIR)/main

# Phony Targets
.PHONY: all run clean debug debug-run release release-run

all: debug

run: debug-run

clean: 
	@rm -rf $(DEBUG_DIR) $(RELEASE_DIR)

debug: $(DEBUG_BIN)

debug-run: debug 
	@"$(DEBUG_BIN)"

release: $(RELEASE_BIN)

release-run: release 
	@"$(RELEASE_BIN)"

$(DEBUG_DIR):
	@mkdir -p $@

$(RELEASE_DIR):
	@mkdir -p $@

# Compilation Rules
define COMPILE_RULE
$1/$(notdir $(2:.c=.o)): $(2)
	@mkdir -p $$(@D)
	$3 $4 -MMD -MP -c $$< -o $$@
endef

# Compilation
$(foreach src,$(SRCS),\
	$(eval $(call COMPILE_RULE,$(DEBUG_OBJ_DIR),$(src),$(CC),$(DEBUG_FLAGS))))

$(foreach src,$(SRCS),\
	$(eval $(call COMPILE_RULE,$(RELEASE_OBJ_DIR),$(src),$(CC),$(RELEASE_FLAGS))))

# Linking
$(DEBUG_BIN): $(DEBUG_OBJS) | $(DEBUG_DIR)
	$(CC) $(DEBUG_FLAGS) $^ $(LIBS) -o $@

$(RELEASE_BIN): $(RELEASE_OBJS) | $(RELEASE_DIR)
	$(CC) $(RELEASE_FLAGS) $^ $(LIBS) -o $@

# Include Dependencies
-include $(DEBUG_DEPS)
-include $(RELEASE_DEPS)