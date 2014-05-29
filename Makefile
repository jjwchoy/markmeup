all: build/libmarkmeup.a

.PHONY: clean

SRCS=\
	 src/builder.h \
	 src/builder.c

build/builder.o: src/builder.h src/builder.c
	$(CC) -o build/builder.o -c $(CFLAGS) src/builder.c

build/html-parser.o: src/html-parser.h src/html-parser.c
	$(CC) -o build/html-parser.o -c $(CFLAGS) src/html-parser.c

build/libmarkmeup.a: build/builder.o build/html-parser.o
	ar rcs $@ $^

clean:
	rm build/*
