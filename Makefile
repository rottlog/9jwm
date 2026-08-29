PREFIX ?= /usr/local

CFLAGS = -std=c11 -Wall -Wextra
LDFLAGS = -lxcb -lxcb-keysyms -lxcb-cursor

SRC = main.c
OBJ = main.o

all: 9jwm

9jwm: $(OBJ)
			$(CC) -o 9jwm $(OBJ) $(LDFLAGS)

$(OBJ): $(SRC)
			$(CC) -c $(SRC) -o $(OBJ)

clean:
	rm -f 9jwm $(OBJ)

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -Dm755 9jwm $(DESTDIR)$(PREFIX)/bin/9jwm

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/9jwm

.PHONY: all clean install uninstall
