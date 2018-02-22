CC = $(CROSS)gcc
LINK = $(CROSS)gcc
AR = $(CROSS)ar
CFLAGS = -Ofast -g -fPIC -DFP_TYPE=float -lm -Wno-unused-result

TARGET_A = build/libutaust.a
OBJS = build/utaust.o

all: $(TARGET_A)

test: build/test
	build/test

build/test: $(TARGET_A) test/test.c
	$(CC) $(CFLAGS) -o build/test test/test.c $(TARGET_A)

$(TARGET_A): $(OBJS)
	$(AR) -rv -o $(TARGET_A) $(OBJS)

build/%.o: %.c
	mkdir -p build
	$(CC) $(CFLAGS) -o build/$*.o -c $*.c

clean:
	rm build/*
