PREFIX ?= /usr/local

CFLAGS += -std=c11 -Wall -Wextra
LDFLAGS = -lxcb -lxcb-keysyms -lxcb-cursor

SRCDIR = src
WRKDIR = wrk

SRC = $(wildcard $(SRCDIR)/*.c)
OBJ = ${patsubst $(SRCDIR)/%.c, $(WRKDIR)/%.o, $(SRC)}

all: 9jwm

$(WRKDIR)/%.o: $(SRCDIR)/%.c | $(WRKDIR)
	$(CC) -c $(CFLAGS) $< -o $@

$(WRKDIR):
	mkdir -p $(WRKDIR)

9jwm: $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -rf $(WRKDIR) 9jwm

install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -Dm755 9jwm $(DESTDIR)$(PREFIX)/bin/9jwm

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/9jwm

.PHONY: all clean install uninstall
