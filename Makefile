CC = gcc
RGBDS = C:/Users/Schueler/Downloads/rgbds
SRC_FILES = src/main.c src/emulator.c src/cpu.c src/mmu.c src/lcd.c src/input.c src/timer.c src/mbc.c src/debug.c
CFLAGS = -g -fstack-protector -Wall -Wextra  -Iinclude `sdl2-config --cflags --libs`

all: emulator

boot:
	$(RGBDS)/rgbasm -oboot.obj src/boot.asm
	$(RGBDS)/rgblink -oboot.bin boot.obj

emulator:
	$(CC) -o emulator $(SRC_FILES) $(CFLAGS)

clean:
	rm -f emulator 
	rm -f $(OBJS)
