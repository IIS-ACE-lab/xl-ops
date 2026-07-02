# Compiler
CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++17 -O3 -fmax-errors=1
#CXXFLAGS := -Wall -Wextra -std=c++17 -g -fmax-errors=1
LDFLAGS := -lm -lmpfi -lmpfr -lgmp

# Directories
BUILD_DIR := build
BIN_DIR := bin

# All source files
SRCS := $(wildcard *.cpp)

# List ALL files that contain a main()
MAIN_SRCS := XL-test.cpp

# Everything else is shared code
COMMON_SRCS := $(filter-out $(MAIN_SRCS), $(SRCS))

# Object files
COMMON_OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(COMMON_SRCS))

# Targets (derived automatically from MAIN_SRCS)
TARGETS := $(basename $(MAIN_SRCS))
BINS := $(addprefix $(BIN_DIR)/,$(TARGETS))

# Default
all: $(BINS)

# Rule to build each binary
$(BIN_DIR)/%: $(COMMON_OBJS) $(BUILD_DIR)/%.o
	@mkdir -p $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

# Compile
$(BUILD_DIR)/%.o: %.cpp $(wildcard *.h)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean

