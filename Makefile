CXX = g++
SRC_FILES = src/main.cpp src/emulator.cpp src/cpu.cpp src/mmu.cpp
CFLAGS = -Wall -Wextra -Werror -Iinclude

all: emulator

emulator:
	$(CXX) -o emulator $(SRC_FILES) $(CFLAGS)

clean:
	rm -f emulator 
	rm -f $(OBJS)