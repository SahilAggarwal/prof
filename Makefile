INCDIR = -Iinc
SRCDIR = src
OBJDIR = obj
BINDIR = bin

CC = gcc
TARGET = perf-prof

RM = rm -f

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(OBJDIR)/shared.o $(OBJDIR)/main.o $(OBJDIR)/probe.o $(OBJDIR)/cpu_map.o $(OBJDIR)/thread_map.o $(OBJDIR)/event_map.o $(OBJDIR)/perf_event.o $(OBJDIR)/mmap_page.o $(OBJDIR)/output.o $(OBJDIR)/probe_buff.o
	$(CC) $(INCDIR) $^ -g -lpthread -o $@

$(OBJDIR)/shared.o: $(SRCDIR)/shared.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/probe.o: $(SRCDIR)/probe.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/cpu_map.o: $(SRCDIR)/cpu_map.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/thread_map.o: $(SRCDIR)/thread_map.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/event_map.o: $(SRCDIR)/event_map.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/perf_event.o: $(SRCDIR)/perf_event.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/mmap_page.o: $(SRCDIR)/mmap_page.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/output.o: $(SRCDIR)/output.c
	$(CC) $(INCDIR) -c $^ -g -o $@

$(OBJDIR)/probe_buff.o: $(SRCDIR)/probe_buff.c
	$(CC) $(INCDIR) -c $^ -g -o $@

clean:
	rm obj/* 
	rm *.text
