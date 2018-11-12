# My Makefile
# Author: Janaina Kaufmann (ECS), Vadim Budagov (ECS)

CC      = gcc
CFLAGS  = -Wall -o2
LDFLAGS = -g -lpthread

SRCDIR  = src
OBJDIR  = obj
BINDIR  = bin
TESTDIR = test

SRC     = $(wildcard $(SRCDIR)/*.c)
OBJ     = $(patsubst $(SRCDIR)/%,$(OBJDIR)/%,$(SRC:.c=.o))
DEPS    = $(OBJ:.o=.d)
# ifdef cond.c
TARGETS = $(TESTDIR)/main

RED = \033[0;31m
NC = \033[0m

all: $(TARGETS)
-include $(DEPS)

$(OBJDIR)/%.d: $(SRCDIR)/%.c
	@echo -e "${RED}Eine Abhaengigkeit fuer $< erstellt! ${NC}"
	@mkdir -p $(@D)
	$(CC) -MM -MT "$@ $(patsubst %.d,%.o,$@)" -MF $@ $<


$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@echo -e "${RED}Ein Objekt fuer $< erstellt! ${NC}"
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
# ifdef cond.c
$(TESTDIR)/main: $(OBJDIR)/main.o $(OBJDIR)/my_mutex.o $(OBJDIR)/FIFO.o
	@echo -e "${RED}EXECUTABLE von $< ${NC}"
	@mkdir -p $(@D)
	$(CC) $(LDFLAGS) -o $@ $^

install:
	@echo -e "${RED}Installation vorbereitet! ${NC}"
	@mkdir -p $(BINDIR)
	mv $(TESTDIR)/* $(BINDIR)/

clean:
	@echo -e "${RED}Temporaere Dateien geloescht! ${NC}"
	rm -rf $(TESTDIR) $(OBJDIR)

delete: clean
	@echo -e "${RED}BINDIR geloescht! ${NC}"
	rm -r -f $(BINDIR)

run:
	bin/./main
