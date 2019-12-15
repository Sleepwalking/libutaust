PREFIX = /usr
CC ?= gcc
AR ?= ar

OBJS = build/utaust.o
CONFIG = Debug

ARFLAGS = -rv
CFLAGS_COMMON = -DFP_TYPE=float -std=c99 -Wall -Wno-unused-result -fPIC
ifeq ($(CXX), emcc)
  CFLAGS_DBG = $(CFLAGS_COMMON) -O1 -g -D_DEBUG
  CFLAGS_REL = $(CFLAGS_COMMON) -O3
else
  CFLAGS_DBG = $(CFLAGS_COMMON) -Og -g -D_DEBUG
  CFLAGS_REL = $(CFLAGS_COMMON) -Ofast
endif
ifeq ($(CONFIG), Debug)
  CFLAGS = $(CFLAGS_DBG)
else
  CFLAGS = $(CFLAGS_REL)
endif

TARGET_A = build/libutaust.a

default: $(TARGET_A)

ifeq ($(CXX), emcc)
test: build/test.js
	node build/test.js
else
test: build/test
	build/test
endif

build/test: $(TARGET_A) test/test.c
	$(CC) $(CFLAGS) -o build/test test/test.c $(TARGET_A)

build/test.js: $(TARGET_A) test/test.c
	$(CC) $(CFLAGS) -o build/test.js test/test.c $(TARGET_A)

$(TARGET_A): $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET_A) $(OBJS)

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$*.o -c $*.c

install: $(TARGET_A)
	mkdir -p $(PREFIX)/lib $(PREFIX)/include/libutaust
	cp $(TARGET_A) $(PREFIX)/lib
	cp utaust.h $(PREFIX)/include/libutaust

clean:
	rm build/*
