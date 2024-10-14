# Makefile for CHIP-8 Emulator

CC = gcc
CFLAGS = -Wall -Wextra -O2 `sdl2-config --cflags` -Iinclude
LDFLAGS = `sdl2-config --libs` -lm
TARGET = chip8_emulator

SRCDIR = src
INCDIR = include
BUILDDIR = build

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRC))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILDDIR) $(TARGET)

.PHONY: all clean
