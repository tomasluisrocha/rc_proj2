# Makefile for the project

# Parameters
CC := gcc
CFLAGS := -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200112L

BIN := bin
SRC := src

SRCS := $(wildcard $(SRC)/*.c)
HDRS := $(wildcard $(SRC)/*.h)

TARGET := $(BIN)/download
TEST_URL ?= ftp://mirrors.up.pt/debian/README.html

# All
.PHONY: all
all: $(TARGET)

$(TARGET): $(SRCS) $(HDRS) | $(BIN)
	$(CC) $(CFLAGS) -o $@ $(SRCS)

# Run with default or custom URL: make run TEST_URL=ftp://...
.PHONY: run
run: $(TARGET)
	./$(TARGET) "$(TEST_URL)"

# Create bin directory
$(BIN):
	mkdir -p $(BIN)

# Clean
.PHONY: clean
clean:
	rm -rf $(BIN)
