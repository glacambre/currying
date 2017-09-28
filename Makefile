CC = gcc
CFLAGS = -O0
LFLAGS = 

C_DIR := src
C_FILES := $(shell find $(C_DIR) -name '*.c')
H_DIR := include
O_DIR := build
O_FILES := $(patsubst $(C_DIR)%.c,$(O_DIR)%.o,$(C_FILES))
BIN_DIR := bin

currying: $(O_FILES) $(BIN_DIR)
	$(CC) $(LFLAGS) $(O_FILES) -o $(BIN_DIR)/$@

$(O_DIR)/%.o: $(C_DIR)/%.c | $(O_DIR) 
	$(CC) $(CFLAGS) -I$(H_DIR) -c $< -o $@

$(O_DIR):
	mkdir -p $(O_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)
