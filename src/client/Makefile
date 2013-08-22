CFLAGS+=-I../include

all: client

client: main.o
	gcc $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o client

install:
	cp client ../../dist/bin/

