CC=gcc
CFLAGS=-W -Wall -g
LDFLAGS=
EXEC=simu
SRC= core.c cache.c snoop.c bus.c init.c simu.c
OBJ= $(SRC:.c=.o)

all: $(EXEC)

simu: $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

%.o: %.c types.h
	$(CC) -o $@ -c $< $(CFLAGS)

.PHONY: clean mrproper

clean:
	rm -rf *.o simu

mrproper: clean
	rm -rf $(EXEC)
