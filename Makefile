# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -g -Iinclude
LDFLAGS = 

# Directories and files
SRC_DIR = src
INCLUDE_DIR = include
SRCS = $(wildcard $(SRC_DIR)/*.c)
HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
BIN = make_file_system
OUTPUT_DIR = output

# Default target
all: $(BIN)

# Build target
$(BIN): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -o $(BIN) $(SRCS) $(LDFLAGS)


# Run target
run: $(BIN)
	@./$(BIN)

# Clean target
clean:
	rm -f $(BIN)
	rm -rf $(OUTPUT_DIR)/*.bin
	rm -rf $(OUTPUT_DIR)

# Prepare environment (e.g., create output directory if missing)
prepare:
	@mkdir -p $(OUTPUT_DIR)

# Phony targets
.PHONY: all run clean prepare
