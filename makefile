PREFIX = /usr
CC = $(CROSS)gcc
LINK = $(CROSS)gcc
AR = $(CROSS)ar

OBJS = build/utaust.o

ARFLAGS = -rv
CFLAGS_COMMON = -DFP_TYPE=float -std=c99 -Wall -Wno-unused-result -fPIC
CFLAGS_DBG = $(CFLAGS_COMMON) -Og -g
CFLAGS_REL = $(CFLAGS_COMMON) -Ofast
CFLAGS = $(CFLAGS_DBG)

TARGET_A = build/libutaust.a

default: $(TARGET_A)

test: build/test
	build/test

build/test: $(TARGET_A) test/test.c
	$(CC) $(CFLAGS) -o build/test test/test.c $(TARGET_A)

$(TARGET_A): $(OBJS)
	$(AR) $(ARFLAGS) -o $(TARGET_A) $(OBJS)

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$*.o -c $*.c

install: $(TARGET_A)
	mkdir -p $(PREFIX)/lib $(PREFIX)/include/libutaust
	cp $(TARGET_A) $(PREFIX)/lib
	cp utaust.h $(PREFIX)/include/libutaust

clean:
	rm build/*
