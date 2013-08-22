CFLAGS+=-pthread -I../include -g
LDFLAGS+=-pthread

all: server

server: main.o thr_channel.o thr_list.o medialib.o mytbf.o
	$(CC) $(LDFLAGS) -o $@ $^

clean:
	rm -rf *.o server

install:
	cp server ../../dist/bin/

