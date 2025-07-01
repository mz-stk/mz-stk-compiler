CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -g
LDFLAGS = 

TARGET = mzstk_to_ast
SRC = mzstk_to_ast.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

.PHONY: all clean test debug install
