INCDIR = -Iinc
SRCDIR = src
OBJDIR = obj
BINDIR = bin

CC = gcc
TARGET = perf-prof

RM = rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJDIR)/shared.o $(OBJDIR)/main.o $(OBJDIR)/probe.o $(OBJDIR)/event_list.o $(OBJDIR)/perf_event.o $(OBJDIR)/mmap_pages.o
	$(CC) $(INCDIR) $^ -g -o $@

$(OBJDIR)/shared.o: $(SRCDIR)/shared.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/probe.o: $(SRCDIR)/probe.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/event_list.o: $(SRCDIR)/event_list.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/perf_event.o: $(SRCDIR)/perf_event.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/mmap_pages.o: $(SRCDIR)/mmap_page.c
	$(CC) $(INCDIR) -c $^ -g -o $@

clean:
	rm obj/* 
