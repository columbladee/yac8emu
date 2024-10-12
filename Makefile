CC = gcc
CFLAGS = -Wall -Wextra -O2 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs` -lm
TARGET = chip8_emulator

SRCS = main.c chip8.c graphics.c input.c audio.c logger.c sdl_wrapper.c memory.c
OBJS = $(SRCS:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean