##
# Totally not Serde
#
# @file
# @version 0.1

LD          := ld
CC          := gcc
RMDIR       := rm -rf
RM          := rm -f
MKDIR       := mkdir

SRC         := ./src
OBJ         := ./obj
BIN         := ./bin
INCLUDE     := ./include
LIB         := ./generi.c

SRCS        := $(wildcard $(SRC)/*.c) $(wildcard $(LIB)/src/*.c)
OBJS        := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))
TARGET      := $(BIN)/tests

LDFLAGS     :=
CFLAGS      := -I$(INCLUDE) -I$(LIB)/include
DEBUGFLAGS  := -O0 -ggdb

.PHONY: all clean run debug

all: $(TARGET)

run: all
	./$(TARGET)

debug: CFLAGS := $(CFLAGS) $(DEBUGFLAGS)
debug: all

$(TARGET): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ) $(BIN):
	$(MKDIR) $@

clean:
	$(RMDIR) $(BIN) $(OBJ)

# end
