CXX = gcc
SRC_FILES = src/main.c src/emulator.c src/cpu.c src/mmu.c
CFLAGS = -Wall -Wextra  -Iinclude

all: emulator

emulator:
	$(CXX) -o emulator $(SRC_FILES) $(CFLAGS)

clean:
	rm -f emulator 
	rm -f $(OBJS)