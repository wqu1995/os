CC := g++
CFLAGS := -std=c++11
SRC = main.cpp utils.cpp process.cpp page.cpp
EXEC := test

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
.PHONY: clean
clean:
	rm -f mmu *~
