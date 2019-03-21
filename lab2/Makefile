CC := g++
CFLAGS := -std=c++11
SRC = main.cpp utils.cpp process.cpp event.cpp schedulers.cpp
EXEC := a

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
.PHONY: clean
clean:
	rm -f a *~
