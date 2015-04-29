INCDIR = -Iinc
SRCDIR = src
OBJDIR = obj
BINDIR = bin

CC = gcc
TARGET = $(BINDIR)/perf-prof
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJS = $(SOURCES:.c=.o)
OBJECTS = $(patsubst $(SRCDIR)%,$(OBJDIR)%,$(OBJS))


$(TARGET):$(OBJECTS)
	$(CC) $(OBJECTS) -lpthread -o $(TARGET)

$(OBJDIR)/%.o:$(SRCDIR)/%.c
	$(CC) $(INCDIR) -c $< -o $@


clean:
	rm $(OBJDIR)/*
