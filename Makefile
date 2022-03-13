CC = gcc
SRC_FILES = src/main.c src/emulator.c src/cpu.c src/mmu.c src/lcd.c
CFLAGS = -Wall -Wextra  -Iinclude `sdl2-config --cflags --libs` -g

all: emulator

emulator:
	$(CC) -o emulator $(SRC_FILES) $(CFLAGS)

clean:
	rm -f emulator 
	rm -f $(OBJS)
