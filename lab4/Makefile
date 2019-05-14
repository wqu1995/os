CC := g++
CFLAGS := -O3 -std=c++11
SRC = main.cpp utils.cpp schedulers.cpp
EXEC := iosched

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $(EXEC) $(SRC)
.PHONY: clean
clean:
	rm -f iosched *~
