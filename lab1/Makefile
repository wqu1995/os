CC := g++
CFLAGS := -std=c++11
SRC = main.cpp error.cpp parser.cpp symbol.cpp token.cpp utils.cpp
EXEC := linker

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
.PHONY: clean
clean:
	rm -f linker *~
