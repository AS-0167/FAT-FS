# # Compiler and flags
# CC = gcc
# CFLAGS = -Wall -Wextra -g -Iinclude
# LDFLAGS = 

# # Directories and files
# SRC_DIR = src
# INCLUDE_DIR = include
# SRCS = $(wildcard $(SRC_DIR)/*.c)
# HEADERS = $(wildcard $(INCLUDE_DIR)/*.h)
# BIN = make_file_system
# OUTPUT_DIR = output

# # Default target
# all: $(BIN)

# # Build target
# $(BIN): $(SRCS) $(HEADERS)
# 	$(CC) $(CFLAGS) -o $(BIN) $(SRCS) $(LDFLAGS)


# # Run target
# run: $(BIN)
# 	@./$(BIN)

# # Clean target
# clean:
# 	rm -f $(BIN)
# 	rm -rf $(OUTPUT_DIR)/*.bin
# 	rm -rf $(OUTPUT_DIR)

# # Prepare environment (e.g., create output directory if missing)
# prepare:
# 	@mkdir -p $(OUTPUT_DIR)

# # Phony targets
# .PHONY: all run clean prepare


CC = gcc
CFLAGS = -Wall -g
SRC = src/fat.c src/disk_operations.c src/file.c src/folder.c src/directory.c src/utils.c src/main.c src/meta_data.c
OBJ = $(SRC:.c=.o)
EXEC = main

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(EXEC)