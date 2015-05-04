BINDIR	= ../bin
CC	= gcc
TARGET	= test

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): test.c
	$(CC) test.c -lpthread -o $@

clean:
	rm *.text

