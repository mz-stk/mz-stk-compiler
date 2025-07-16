CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
SRC_DIR = src
OBJ_DIR = obj

SOURCES = $(wildcard $(SRC_DIR)/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SOURCES))
EXECUTABLE = mzstk

PREFIX = /usr/local
BIN_DIR = $(PREFIX)/bin

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

install: $(EXECUTABLE)
	install -d $(BIN_DIR)
	install -m 755 $(EXECUTABLE) $(BIN_DIR)

clean:
	rm -rf $(OBJ_DIR) $(EXECUTABLE)

uninstall:
	rm -f $(BIN_DIR)/$(EXECUTABLE)

.PHONY: all clean install uninstall
