CC := gcc
CFLAGS := -Wall -Wextra -pthread -g
SRC := $(wildcard src/*.c)
OBJ := $(SRC:.c=.o)
TARGET := server

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) $(TARGET)
