# ================================
# Multithreaded Web Server Makefile
# Usable across Unix-like Systems
# ================================

# Compiler and Flags
CC      := gcc
CFLAGS  := -Wall -Wextra -Werror -pthread -g
INCLUDES := -Iinclude

# Directories
SRC_DIR := src
BIN_DIR := bin

# Target binary
TARGET := $(BIN_DIR)/server

# Source files (all .c files in src/)
SRCS := $(wildcard $(SRC_DIR)/*.c)

# Object files (convert src/file.c → bin/file.o)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.o,$(SRCS))


# ================================
# Default Target
# ================================
all: $(TARGET)


# ================================
# Build Target
# ================================
$(TARGET): $(BIN_DIR) $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Rule to compile each .c into .o in bin/
$(BIN_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@


# ================================
# Directory Creation
# ================================
$(BIN_DIR):
	mkdir -p $(BIN_DIR)


# ================================
# Utility Targets
# ================================
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf $(BIN_DIR)

rebuild: clean all


# ================================
# Thread + Networking Tests
# ================================
test-pthread:
	$(CC) $(CFLAGS) tests/pthread_test.c -o tests/pthread_test -pthread
	./tests/pthread_test

test-stress:
	$(CC) $(CFLAGS) tests/pthread_stress_test.c -o tests/pthread_stress_test -pthread
	./tests/pthread_stress_test


# ================================
# Mark phony targets
# ================================
.PHONY: all run clean rebuild test-pthread test-stress
