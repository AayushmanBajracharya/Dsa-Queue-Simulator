# Makefile for Traffic Simulator

# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g
LDFLAGS = -lSDL2

# Directories
SRC_DIR = .
OBJ_DIR = obj
BIN_DIR = bin

# Source files
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/traffic_simulator.c $(SRC_DIR)/generator.c $(SRC_DIR)/traffic_gui.c
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))

# Output binary
TARGET = $(BIN_DIR)/traffic_simulator

# Main target
all: directories $(TARGET)

# Create necessary directories
directories:
	mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Linking
$(TARGET): $(OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compiling
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for simulator.c if needed
$(OBJ_DIR)/simulator.o: $(SRC_DIR)/traffic_simulator.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

# Run
run: all
	$(TARGET)

# Non-graphical run
text: all
	$(TARGET) -g 0

# Phony targets
.PHONY: all directories clean run text