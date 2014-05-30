all: build/libmarkmeup.a build/test

.PHONY: clean

CFLAGS += -I/usr/include/libxml2 -g

SRCS=\
	 src/builder.h \
	 src/builder.c

build/builder.o: src/builder.h src/builder.c
	$(CC) -o build/builder.o -c $(CFLAGS) src/builder.c

build/html-parser.o: src/html-parser.h src/html-parser.c
	$(CC) -o build/html-parser.o -c $(CFLAGS) src/html-parser.c

build/libmarkmeup.a: build/builder.o build/html-parser.o
	ar rcs $@ $^

build/test: src/test.c build/libmarkmeup.a
	$(CC) -o $@ src/test.c build/libmarkmeup.a -L/usr/lib/libxml2 -lxml2

clean:
	rm build/*
