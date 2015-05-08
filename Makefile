INCDIR = inc
SRCDIR = src
OBJDIR = obj
BINDIR = bin

ARCH := $(shell getconf LONG_BIT)

CC = gcc
TARGET = $(BINDIR)/perf-prof
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SOURCES:.c=.o)
OBJECTS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(OBJS))


$(TARGET):$(OBJECTS)
	$(CC) $(OBJECTS) -lpthread -o $(TARGET)
ifeq ($(ARCH),64)
	./syscallent.sh /usr/include/asm/unistd_64.h > $(INCDIR)/syscallent.h
else 
	./syscallent.sh /usr/include/asm/unistd_32.h > $(INCDIR)/syscallent.h
endif

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	$(CC) -I$(INCDIR) -c $< -o $@


clean:
	rm $(OBJDIR)/*
